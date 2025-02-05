#ifndef LIBMETA_VARIANT_HPP
#define LIBMETA_VARIANT_HPP

#include "exports.h"
#include "refl/ref.hpp"
#include "refl/type.hpp"

#include <utility>
#include <stdexcept>

/**
 * 期望 Any 可以:
 * 1. 从任意类型 T 构造
 * 2. 从任意类型 T 赋值
 * 3. 有值时可以转换为 T& / T*
 * 4. 当类型 T&/T* 可以赋值给类型 U&/U* 时，值也可以转换为 U&/U*
 * 5. 可以获取当前存储值的类型 TypeRef
 * 6. 简单的算数类型值之间可以转换，也可以转换成字符串 std::string, 但不能由 std::string 转换成数字
 * 7. 根据存储类型区分复制操作
 * 8. 可判空，与nullptr比较，提供Valid函数
 * 
 * 使用限制：
 * 1. 只能存储可以拷贝构造的值类型
 * 2. 或者 Ref<T>
 */

namespace Meta
{
    namespace details {

        // 小对象查看器， 后续可以添加其他类型
        union alignas(max_align_t) BufferView {
            s8   _s8;
            s16  _s16;
            s32  _s32;
            s64  _s64;
            u8   _u8;
            u16  _u16;
            u32  _u32;
            u64  _u64;
            f32  _f32;
            f64  _f64;
            cstr _cstr;
        };

        struct LIBMETA_API AnyOps {
            void  (*Destroy)(void*) = nullptr;
            void* (*Clone)(void*, void*) = nullptr;

            // helpers
            template <typename T>
            static AnyOps Of(std::enable_if_t<(sizeof(BufferView) < sizeof(T))> * = nullptr)
            {
                return AnyOps {
                    .Destroy = +[](void* data) { delete (T*)data; },
                    .Clone = +[](void*, void *data)->void* { return new T{*(T*)data}; },
                };
            }

            template <typename T>
            static AnyOps Of(std::enable_if_t<(sizeof(BufferView) >= sizeof(T))> * = nullptr)
            {
                return AnyOps {
                    .Destroy = +[](void* data) { ((T*)data)->~T(); },
                    .Clone = +[](void* stack, void *data)->void* { return new (stack)T{*(T*)data}; },
                };
            }

            template <typename T>
            static AnyOps OfRef()
            {
                // TODO:
                return {};
            }

            static AnyOps Empty();
        };

        // class LIBMETA_API AnyBase {
        // public:
        //     virtual ~AnyBase();

        //     virtual TypePtr GetType() = 0;

        //     virtual AnyBase* Clone(void *stack) = 0;

        //     virtual void*    Ptr() = 0;
        // };

        // template <typename T>
        // class AnyDerivedStack: public AnyBase {
        //     T value_;
        // public:
        //     template<typename U>
        //     AnyDerivedStack(U t, std::enable_if_t<std::is_nothrow_constructible_v<T, U>> * = nullptr): value_(t) {}
        //     ~AnyDerivedStack() override = default;
        //     TypePtr  GetType() override { return TypeOf<T>(); }
        //     AnyBase *Clone(void *stack) override { return new(stack) AnyDerivedStack(value_); }
        //     void    *Ptr() override { return &value_; }
        // };

        // template <typename T>
        // class AnyDerivedHeap: public AnyBase {
        //     T value_;
        // public:
        //     template<typename U>
        //     AnyDerivedHeap(U t, std::enable_if_t<std::is_nothrow_constructible_v<T, U>> * = nullptr): value_(t) {}
        //     ~AnyDerivedHeap() override = default;
        //     TypePtr  GetType() override { return TypeOf<T>(); }
        //     AnyBase *Clone(void *) override { return new AnyDerivedHeap(value_); }
        //     void    *Ptr() override { return &value_; }
        // };
    }

    class LIBMETA_API Any
    {
        static constexpr size_t BufferSize = sizeof(details::BufferView);
    public:
        // MARK: Constructors
        // Empty
        Any () {
            ops_ = details::AnyOps::Empty();
        }

        // Ref
        template <typename T>
        Any (T value)
        {
            Construct(value);
        }

        // Ref<T>
        template <typename T>
        Any (Ref<T> ref)
        {
            // TODO: 
        }

        Any (const Any &var) {
            Assign(var);
        }

        Any (Any &&var) {
            Assign(var);
        }

        ~Any ()
        {
            Destroy();
        }

        // MARK: Assignment
        template <typename T>
        Any &operator= (T t)
        {  // New Ref
            Destroy();
            Construct(t);
            return *this;
        }
        template <typename T>
        Any &operator= (Ref<T> ref)
        {  // Const Ref
            Destroy();
            Assign(std::move(ref));
            return *this;
        }
        Any &operator= (const Any &var)
        {
            Destroy();
            Assign(var);
            return *this;
        }
        Any &operator= (Any &&var)
        {
            Destroy();
            Assign(var);
            return *this;
        }

        template <typename T>
        T *ValuePtr () const
        {
            assert(Valid());
            if (type_id_ != GetTypeId<T>())
                return nullptr;
            return (T*)data_;
        }

        template <typename T>
        T &ValueRef () const
        {
            assert(Valid());
            if (type_id_ != GetTypeId<T>())
                throw std::bad_cast();
            return *(T*)data_;
        }

        template<typename T>
        inline operator T*() const {
            return ValuePtr<T>();
        }

        template<typename T>
        inline operator T() const {
            return *ValuePtr<T>();
        }

        bool Valid () const
        {
            return type_id_ != NULL_TYPE_ID;
        }

        operator bool () const
        {
            return Valid ();
        }

        bool operator== (decltype (nullptr)) const
        {
            return !Valid ();
        }

        bool operator!= (decltype (nullptr)) const
        {
            return Valid ();
        }

        inline TypePtr Type () const
        {
            return TypeOf(type_id_);
        }

    private:
        // MARK: Helpers
        bool IsSmallObj() const 
        {
            return data_ == &buffer_;
        }

        void Destroy()
        {
            if (data_) {
                ops_.Destroy(data_);
            }
        }

        template <typename T>
        void Construct(const T &t)
        {
            type_id_ = GetTypeId<T>();
            ops_ = details::AnyOps::Of<T>();
            if constexpr (sizeof(T) < BufferSize) {
                data_ = new(&buffer_)T{t};
            } else {
                data_ = new T{t};
            }
        }

        void Assign(const Any &any)
        {
            if (any.Valid()) {
                type_id_ = any.type_id_;
                ops_ = any.ops_;
                data_ = any.ops_.Clone(&buffer_, any.data_);
            }
        }

        void Assign(Any &&any)
        {
            if (any.Valid()) {
                type_id_ = any.type_id_;
                ops_ = any.ops_;
                if (any.IsSmallObj()) {
                    data_ = any.ops_.Clone(&buffer_, any.data_);
                } else {
                    std::swap(data_, any.data_);
                }
            }
        }

        template <typename T>
        void Assign(Ref<T> &&ref)
        {
            // TODO:
        }

        details::BufferView                  buffer_; // for small object
        void                                *data_{};
        TypeId                               type_id_{NULL_TYPE_ID};
        details::AnyOps                      ops_;
    };
}  // namespace Meta

#endif /* LIBMETA_VARIANT_HPP */
