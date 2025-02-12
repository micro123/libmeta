#ifndef LIBMETA_VARIANT_HPP
#define LIBMETA_VARIANT_HPP

#include <stdexcept>
#include <typeinfo>
#include <utility>
#include "exports.h"
#include "refl/ref.hpp"
#include "refl/type.hpp"

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
    namespace details
    {

        // 小对象查看器， 后续可以添加其他类型
        union alignas (max_align_t) BufferView
        {
            s8     _s8;
            s16    _s16;
            s32    _s32;
            s64    _s64;
            u8     _u8;
            u16    _u16;
            u32    _u32;
            u64    _u64;
            f32    _f32;
            f64    _f64;
            cstr   _cstr;
            void  *_ptr;
            void **_pptr;

            char _pad[sizeof (void *) * 8];
        };

        // check operator==
        template <typename T, typename = void>
        struct has_equal_op : std::false_type {
        };
        template <typename T>
        struct has_equal_op<T, std::void_t<decltype (std::declval<T> () == std::declval<T> ())>> : std::true_type {
        };
        template <typename T>
        constexpr bool has_equal_v = has_equal_op<T>::value;

        // check operator<
        template <typename T, typename = void>
        struct has_less_op : std::false_type {
        };
        template <typename T>
        struct has_less_op<T, std::void_t<decltype (std::declval<T> () < std::declval<T> ())>> : std::true_type {
        };
        template <typename T>
        constexpr bool has_less_v = has_less_op<T>::value;

        struct LIBMETA_API AnyOps {
        private:
            template <typename T>
            static bool IsEqual(void *a, void *b)
            {
                T *ta = static_cast<T *> (a);
                T *tb = static_cast<T *> (b);
                if constexpr (has_equal_v<T>)
                {
                    return *ta == *tb;
                }
                return ta == tb;
            }
            template <typename T>
            static bool IsLess(void *a, void *b)
            {
                T *ta = static_cast<T *> (a);
                T *tb = static_cast<T *> (b);
                if constexpr (has_less_v<T>)
                {
                    return *ta < *tb;
                }
                return ta < tb;
            }

        public:

            void *(*Construct) (void *, void *) = nullptr;
            void (*Destroy) (void *)            = nullptr;
            void *(*Clone) (void *, void *)     = nullptr;
            void *(*Get) (void *)               = nullptr;
            bool (*Equal) (void *, void *)      = nullptr;
            bool (*Less) (void *, void *)       = nullptr;

            // helpers
            template <typename T>
            static AnyOps Of (std::enable_if_t<(sizeof (BufferView) < sizeof (T))> * = nullptr)
            {
                static AnyOps ops {
                        .Destroy = +[] (void *data) { delete static_cast<T *> (data); },
                        .Clone   = +[] (void *, void *data) -> void* { return new T {*static_cast<T *> (data)}; },
                        .Equal   = &AnyOps::IsEqual<T>,
                        .Less    = &AnyOps::IsLess<T>,
                };
                return ops;
            }

            template <typename T>
            static AnyOps Of (std::enable_if_t<(sizeof (BufferView) >= sizeof (T))> * = nullptr)
            {
                static AnyOps ops {
                        .Destroy = +[] (void *data) { static_cast<T *> (data)->~T (); },
                        .Clone   = +[] (void *stack, void *data) -> void* { return new (stack) T {*static_cast<T *> (data)}; },
                        .Equal   = &AnyOps::IsEqual<T>,
                        .Less    = &AnyOps::IsLess<T>,
                };
                return ops;
            }

            template <typename T>
            static AnyOps OfPtr ()
            {
                static AnyOps ops {
                        .Construct = +[] (void *stack, void *data) -> void* { return *(void**)data; },
                        .Clone     = +[] (void *stack, void *data) -> void* { return data; },
                        .Equal   = &AnyOps::IsEqual<T>,
                        .Less    = &AnyOps::IsLess<T>,
                };
                return ops;
            }

            template <typename T>
            static AnyOps OfRef ()
            {
                static AnyOps ops {
                        .Construct = +[] (void *stack, void *data) -> void * { return new (stack) Ref<T> {*static_cast<Ref<T> *> (data)}; },
                        .Destroy   = +[] (void *data) { static_cast<Ref<T> *> (data)->~Ref (); },
                        .Clone     = +[] (void *stack, void *data) -> void     *{ return new (stack) Ref<T> {*static_cast<Ref<T> *> (data)}; },
                        .Get       = +[] (void *data) -> void       *{ return static_cast<Ref<T> *> (data)->Get (); },
                        .Equal   = &AnyOps::IsEqual<T>,
                        .Less    = &AnyOps::IsLess<T>,
                };
                return ops;
            }

            static AnyOps Empty ();
        };
    }  // namespace details

    bool LIBMETA_API AnyCast (const Any &in, TypeId src, Any &out, TypeId dst);

    class LIBMETA_API Any
    {
        static constexpr size_t BufferSize = sizeof (details::BufferView);

    public:
        // MARK: Constructors
        // Empty
        Any ();

        // By Value
        template <typename T>
        Any (T value)
        {
            ConstructValue (std::move (value));
        }

        // Pointer
        template <typename T>
        Any (T *ptr)
        {
            ConstructPointer (ptr);
        }

        Any (cstr s)
        {
            ConstructValue (s);
        }

        // Ref<T>
        template <typename T>
        Any (Ref<T> ref)
        {
            ConstructRef (std::move (ref));
        }

        Any (const Any &var);

        Any (Any &&var) noexcept;

        ~Any ();

        // MARK: Assignment
        template <typename T>
        Any &operator= (T t)
        {  // New Ref
            Destroy ();
            ConstructValue (t);
            return *this;
        }
        template <typename T>
        Any &operator= (T *ptr)
        {
            Destroy ();
            ConstructPointer (ptr);
            return *this;
        }
        Any &operator= (cstr s)
        {
            Destroy ();
            ConstructValue (s);
            return *this;
        }
        template <typename T>
        Any &operator= (Ref<T> ref)
        {  // Const Ref
            Destroy ();
            Assign (std::move (ref));
            return *this;
        }
        Any &operator= (const Any &var);
        Any &operator= (Any &&var) noexcept;

        template <typename T>
        T *ValuePtr () const
        {
            assert (Valid ());
            if constexpr (std::is_same_v<void, T>)
            {
                return Get ();
            }
            else
            {
                if (type_id_ != GetTypeId<T> ())
                {
                    return nullptr;
                }
                return static_cast<T *> (Get ());
            }
        }

        template <typename T>
        T &ValueRef () const
        {
            assert (Valid ());
            if (type_id_ != GetTypeId<T> ())
                throw std::bad_cast ();
            return *static_cast<T *> (Get ());
        }

        template <typename T>
        T Value () const
        {
            assert (Valid ());
            if (type_id_ != GetTypeId<T> ())
            {
                // type must be not null
                auto type = Type ();
                if (!type)
                    throw std::bad_cast ();

                if constexpr (std::is_same_v<str, T>)
                {
                    return type->ToString (*this);
                }
                else
                {
                    Any result;
                    if (!AnyCast (*this, type_id_, result, GetTypeId<T> ()))
                        throw std::bad_cast ();
                    return result.Value<T> ();
                }
            }
            return *ValuePtr<T> ();
        }

        template <typename T>
        [[nodiscard]] inline operator T * () const
        {
            return ValuePtr<T> ();
        }

        template <typename T>
        [[nodiscard]] inline operator T () const
        {
            return Value<T> ();
        }

        [[nodiscard]] inline bool Valid () const noexcept
        {
            return type_id_ != NULL_TYPE_ID;
        }

        [[nodiscard]] operator bool () const noexcept
        {
            return Valid ();
        }

        bool operator== (decltype (nullptr)) const noexcept
        {
            return !Valid ();
        }

        bool operator!= (decltype (nullptr)) const noexcept
        {
            return Valid ();
        }

        [[nodiscard]] TypePtr Type () const;

        static const Any Void;

        template <typename T, typename... Args>
        static Any New (Args &&...args)
        {
            return Any (MakeRef<T> (std::forward<Args> (args)...));
        }

        Any operator() (Any *args, size_t cnt) const;

        template <typename... Args>
        Any operator() (Args &&...args) const
        {
            Any arg_arr[] = {std::forward<Args> (args)..., {}};
            return (*this) (&arg_arr[0], sizeof...(Args));
        }

        template <typename T>
        bool operator== (const T &t) const
        {
            return t == Value<T> ();
        }

        template <typename T>
        bool operator!= (const T &t) const
        {
            return (*this == t);
        }

        bool operator== (const Any &other) const
        {
            if (type_id_ == other.type_id_)
            {
                if (Valid ())
                {
                    return ops_.Equal (Get (), other.Get ());
                }
            }
            return false;
        }

        bool operator!= (const Any &other) const
        {
            return !(*this == other);
        }

    private:
        // special ctor
        explicit Any (TypeId tid);

        // MARK: Helpers
        [[nodiscard]] bool IsSmallObj () const
        {
            return data_ == &buffer_;
        }

        void Destroy () const;

        template <typename T>
        void ConstructValue (T t)
        {
            type_id_ = GetTypeId<T> ();
            ops_     = details::AnyOps::Of<T> ();
            if constexpr (sizeof (T) < BufferSize)
            {
                data_ = new (&buffer_) T {std::move (t)};
            }
            else
            {
                data_ = new T {std::move (t)};
            }
        }

        template <typename T>
        void ConstructPointer (T *ptr)
        {
            type_id_ = GetTypeId<T> ();
            ops_     = details::AnyOps::OfPtr<T> ();
            data_    = ops_.Construct (&buffer_, &ptr);
        }

        template <typename T>
        void ConstructRef (Ref<T> ref)
        {
            type_id_ = GetTypeId<T> ();
            ops_     = details::AnyOps::OfRef<T> ();
            data_    = ops_.Construct (&buffer_, &ref);
        }

        void Assign (const Any &any);

        void Assign (Any &&any);

        template <typename T>
        void Assign (Ref<T> &&ref)
        {
            ConstructRef (ref);
        }

        [[nodiscard]] inline void *Get () const
        {
            return ops_.Get ? ops_.Get (data_) : data_;
        }

        details::BufferView buffer_ {};  // for small object
        void               *data_ {};
        TypeId              type_id_ {NULL_TYPE_ID};
        details::AnyOps     ops_;
    };
}  // namespace Meta

#endif /* LIBMETA_VARIANT_HPP */
