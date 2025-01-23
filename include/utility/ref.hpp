#ifndef LIBMETA_REF_HPP
#define LIBMETA_REF_HPP

#include <exports.h>
#include <atomic>

namespace Meta {
    namespace details {
        // 这个类只负责计数 和 调用析构（Dispose）
        class LIBMETA_API RefCountedBase {
        public:
            // 使这个类无法复制和移动
            RefCountedBase(const RefCountedBase &) = delete;
            RefCountedBase(RefCountedBase &&) = delete;
            RefCountedBase &operator= (const RefCountedBase &) = delete;
            RefCountedBase &operator= (RefCountedBase &&) = delete;

            // 获取当前计数
            s32  RefCount() const { return ref_count_.load(); }

            RefCountedBase();
            virtual ~RefCountedBase();

            // 内部计数加一
            void AddRef();
            // 内部计数减一，当减到0时调用Dispose，然后释放自己
            void UnRef();
        private:
            std::atomic<s32> ref_count_;
        };

        template<typename T>
        concept IsDisposable = requires (T* v) {
            v->Dispose();
        };

        template<typename X, typename Y>
        concept PtrCastable = requires (X *&x, Y* y) {
            x = y;
        };

        template <typename T>
        class RefCounted : public RefCountedBase {
            using DisposeFunc = void(*)(void*);

            RefCounted(): data_(nullptr), dispose_(nullptr) {}

            template <typename U> requires PtrCastable<T, U>
            RefCounted(U *ptr): data_(ptr)
            {
                if constexpr (IsDisposable<T>) {
                    dispose_ = +[](void *data) { if (data) ((T*)data)->Dispose(); };
                } else {
                    dispose_ = +[](void *data) { delete (T*)data; };
                }
            }
            ~RefCounted() {
                if (dispose_)
                    dispose_(data_);
            }
        public:
            inline T *Ptr() { return data_; }
            inline const T *Ptr() const { return data_; }

            static RefCounted<T> *Create() { return new RefCounted<T>(); }

            template <typename U>
            static RefCounted<T> *Create(U* u) { return new RefCounted<T>(u); }

        private:
            T *data_;
            DisposeFunc dispose_;
        };
    }

    

    template <typename T>
    class Ref {
    public:
        // 一般构造函数
        Ref(): ptr_(details::RefCounted<T>::Create()) {}
        template <typename U> requires details::PtrCastable<T, U>
        Ref(U *u): ptr_(details::RefCounted<T>::Create(u)) {}
        ~Ref() { ptr_->UnRef(); }
        Ref(const Ref& other) {
            other.ptr_->AddRef();
            ptr_ = other.ptr_;
        }
        Ref(Ref &&other): ptr_{nullptr} {
            std::swap(ptr_, other.ptr);
        }

        inline T* operator->() { return ptr_->Ptr(); }
        inline const T* operator->() const { return ptr_->Ptr(); }
        inline T& operator*() { 
            return *ptr_->Ptr();
        }
        inline const T& operator*() const { 
            return *ptr_->Ptr();
        }

        inline s32 RefCount() const { return ptr_->RefCount(); }
    private:
        details::RefCounted<T> *ptr_;
    };

    template <typename T, typename ... Args>
    Ref<T> MakeRef(Args && ... args) {
        return Ref<T>(new T{std::forward<Args>(args)...});
    }
}

#endif /* LIBMETA_REF_HPP */
