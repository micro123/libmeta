#ifndef REF_HPP
#define REF_HPP

#include <exports.h>
#include <atomic>

namespace Meta
{
    // fwd
    template <typename T>
    class Ref;

    namespace details
    {
        // 这个类只负责计数 和 调用析构（Dispose）
        class LIBMETA_API RefCountedBase
        {
        public:
            // 使这个类无法复制和移动
            RefCountedBase (const RefCountedBase &)            = delete;
            RefCountedBase (RefCountedBase &&)                 = delete;
            RefCountedBase &operator= (const RefCountedBase &) = delete;
            RefCountedBase &operator= (RefCountedBase &&)      = delete;

            // 获取当前计数
            [[nodiscard]] s32 RefCount () const
            {
                return ref_count_.load ();
            }

            RefCountedBase ();
            virtual ~RefCountedBase ();

            // 内部计数加一
            void AddRef ();
            // 内部计数减一，当减到0时调用Dispose，然后释放自己
            void UnRef ();

        private:
            std::atomic<s32> ref_count_;
        };

        template <typename T>
        concept IsDisposable = requires (T *v) { v->Dispose (); };

        template <typename X, typename Y>
        concept PtrCastable = requires (X *&x, Y *y) { x = y; };

        using DisposeFunc = void (*) (void *);
        template <typename T>
        static inline void DefaultDeleter(void* data)
        {
            if constexpr (IsDisposable<T>)
            {
                static_cast<T *> (data)->Dispose ();
            }
            else
            {
                delete static_cast<T *> (data);
            }
        }

        template <typename T>
        class RefCounted : public RefCountedBase
        {
            template <typename> friend class RefCounted;

            RefCounted () : origin_ (nullptr), data_ (nullptr), dispose_ (nullptr) {}

            template <typename U>
                requires PtrCastable<T, U>
            RefCounted (U *ptr, void *base, DisposeFunc d = {}) : origin_ (base), data_ (ptr), dispose_ (d)
            {
                if (!dispose_)
                    dispose_ = &DefaultDeleter<U>;
            }
            ~RefCounted () override
            {
                if (dispose_)
                    dispose_ (origin_);
            }

        public:
            template <typename U = T>
                requires PtrCastable<U, T>
            inline U *Ptr () const
            {
                return static_cast<U *> (data_);
            }

            template <typename U> requires PtrCastable<U, T>
            RefCounted<U> *Clone()
            {
                AddRef();
                return new RefCounted<U> (static_cast<U *> (data_), origin_, dispose_);
            }

            static RefCounted<T> *Create ()
            {
                return new RefCounted<T> ();
            }

            template <typename U>
            static RefCounted<T> *Create (U *u, void *base, DisposeFunc d)
            {
                return new RefCounted<T> (u, base, d);
            }

        private:
            void       *origin_; // original pointer to object
            T          *data_;
            DisposeFunc dispose_;
        };

        // -> and *
        template <typename T, bool = std::is_void_v<T>>
        struct PtrAccess {
            T* operator->() const noexcept {
                return _Get();
            }

            T& operator*() const noexcept {
                return *_Get();
            }
        private:
            T* _Get() const
            {
                return static_cast<const Ref<T>*>(this)->Get();
            }
        };

        // only ->
        template <typename T>
        struct PtrAccess<T,true> {
            T* operator->() const noexcept {
                return _Get();
            }
        private:
            T* _Get() const
            {
                return static_cast<Ref<T>*>(this)->Get();
            }
        };


    }  // namespace details

    template <typename T>
    class Ref: public details::PtrAccess<T>
    {
        template <typename> friend class Ref;
    public:
        // 一般构造函数
        Ref () : ptr_ (nullptr) {}
        template <typename U>
            requires details::PtrCastable<T, U>
        Ref (U *u, details::DisposeFunc dispose = {})
        : ptr_ (details::RefCounted<T>::Create (u, u, dispose ? dispose : &details::DefaultDeleter<U>))
        {
        }
        ~Ref ()
        {
            if (ptr_)
                ptr_->UnRef ();
        }
        Ref (const Ref &other) : ptr_ (nullptr)
        {
            Assign (other);
        }
        Ref (Ref &&other) : ptr_ {nullptr}
        {
            std::swap (ptr_, other.ptr_);
        }
        template <typename U> requires details::PtrCastable<T, U>
        Ref (const Ref<U> &other)
        {
            if (other.ptr_)
            {
                ptr_ = other.ptr_->template Clone<T> ();
            }
        }

        Ref &operator= (const Ref &other)
        {
            Assign (other);
            return *this;
        }
        Ref &operator= (Ref &&other)
        {
            if (ptr_ != other.ptr_)
            {
                std::swap (ptr_, other.ptr_);
            }
            return *this;
        }
        template <typename U> requires details::PtrCastable<T, U>
        Ref &operator= (const Ref<U> &other)
        {
            if (Get() != other.Get())
            {
                ptr_->UnRef ();
                ptr_ = other.ptr_->template Clone<T> ();
            }
            return *this;
        }

        inline s32 RefCount () const
        {
            return ptr_->RefCount ();
        }

        inline T* Get() const noexcept
        {
            if (ptr_)
                return ptr_->template Ptr<T>();
            return nullptr;
        }

        inline void Release () noexcept
        {
            auto bak = ptr_;
            ptr_ = nullptr;
            if (bak)
                bak->UnRef ();
        }

        template <typename U> requires details::PtrCastable<T, U>
        inline void Reset(U *u, details::DisposeFunc d)
        {
            Release ();
            ptr_ = details::RefCounted<T>::Create (u, u, d ? d : &details::DefaultDeleter<U>);
        }

        operator bool () const
        {
            return ptr_ != nullptr;
        }

        bool operator== (decltype (nullptr)) const
        {
            return !operator bool ();
        }

        bool operator!= (decltype (nullptr)) const
        {
            return operator bool ();
        }

        template <typename U> requires details::PtrCastable<T, U>
        bool operator==(const U *u)
        {
            return ptr_->Ptr() == u;
        }

        template <typename U> requires details::PtrCastable<T, U>
        bool operator!=(const U *u)
        {
            return ptr_->Ptr() != u;
        }

    private:
        void Assign (const Ref &other)
        {
            if (ptr_ == other.ptr_)
                return;
            if (ptr_)
                ptr_->UnRef ();

            ptr_ = nullptr;
            if (other.ptr_ == nullptr)
                return;

            other.ptr_->AddRef ();
            ptr_ = other.ptr_;
        }

        details::RefCounted<T> *ptr_;
    };

    template <typename T, typename... Args>
    Ref<T> MakeRef (Args &&...args)
    {
        return Ref<T> (new T {std::forward<Args> (args)...});
    }

    template <typename T>
    Ref<T> MakeRef (T *obj)
    {
        return {obj};
    }
}  // namespace Meta

#endif /* REF_HPP */
