#ifndef LIBMETA_FIELD_INL
#define LIBMETA_FIELD_INL

#include <cstring>
#include "builders.hpp"

namespace Meta
{
    namespace details
    {
        // normal pointer
        template <typename T>
        class NormalField : public Field
        {
        public:
            NormalField (sview name, T *ptr) : Field (name), ptr_ (ptr)
            {
                assert (ptr_ != nullptr);
            }
            ~NormalField () {}

            bool IsMember () const override
            {
                return false;
            }
            bool IsConst () const override
            {
                return std::is_const_v<T>;
            }
            TypePtr Type () const override
            {
                return TypeOf<T> ();
            }

            Any Get (const Any *, u32 index) const override
            {
                assert (index == 0);
                return Any::RefWrap(ptr_);
            }
            Any Set (const Any *, Any value, u32 index) const override
            {
                assert (index == 0);
                if constexpr (std::is_const_v<T>)
                {
                    return Get (nullptr, index);
                }
                else
                {
                    *ptr_ = value.ValueRef<T> ();
                    return Get (nullptr, index);
                }
            }

        private:
            T *ptr_;
        };

#ifdef EXPLICIT_CLASS_NAME
        template <typename C, typename T>
#else
        template <typename T>
#endif
        class MemberField : public Field
        {
            using Ptr = T (M_T::*);

        public:
#ifndef EXPLICIT_CLASS_NAME
            template <typename C>
#endif
            MemberField (sview name, T (C::*ptr)) : Field (name), ptr_ ((Ptr) ptr)
            {
            }
            ~MemberField () override {}

            bool IsMember () const override
            {
                return true;
            }
            bool IsConst () const override
            {
                return std::is_const_v<T>;
            }
            TypePtr Type () const override
            {
                return TypeOf<T> ();
            }

            Any Get (const Any *object, u32 index) const override
            {
                assert (index == 0);
                auto t_obj = (M_T*) object->ValuePtr<void> ();
                return Any::RefWrap(&(t_obj->*ptr_));
            }
            Any Set (const Any *object, Any value, u32 index) const override
            {
                assert (index == 0);
                auto t_obj = (M_T *) object->ValuePtr<void> ();
                if constexpr (std::is_const_v<T>)
                {
                    return Get (object, index);
                }
                else
                {
                    t_obj->*ptr_ = value.ValueRef<T> ();
                    return Get (object, index);
                }
            }

        private:
            Ptr ptr_;
        };

        template <typename T>
        class NormalArrayField : public Field {
            using Ptr = T(*)[];
        public:
            template <size_t N>
            NormalArrayField(sview name, T (*ptr)[N]): Field(name), ptr_((Ptr) ptr), cnt_(N) {}
            ~NormalArrayField() = default;

            bool IsMember () const override
            {
                return false;
            }
            bool IsConst () const override
            {
                return std::is_const_v<T>;
            }
            bool IsArray() const override
            {
                return true;
            }
            u32 Count() const override
            {
                return (u32)cnt_;
            }
            TypePtr Type () const override
            {
                return TypeOf<T> ();
            }

            Any Get (const Any *, u32 index) const override
            {
                assert (index < cnt_);
                T* ptr = (*ptr_) + index;
                return Any::RefWrap(ptr);
            }
            Any Set (const Any *, Any value, u32 index) const override
            {
                assert (index < cnt_);
                if constexpr (std::is_const_v<T>)
                {
                    return Get (nullptr, index);
                }
                else
                {
                    (*ptr_)[index] = value.ValueRef<T> ();
                    return Get (nullptr, index);
                }
            }

        private:
            Ptr           ptr_;
            size_t  const cnt_;
        };

#ifdef EXPLICIT_CLASS_NAME
        template <typename C, typename T>
#else
        template <typename T>
#endif
        class MemberArrayField : public Field {
            using Ptr = T(M_T::*)[];
        public:
#ifndef EXPLICIT_CLASS_NAME
            template <typename C, size_t N>
#else
            template <size_t N>
#endif
            MemberArrayField(sview name, T(C::*ptr)[N]): Field(name), ptr_((Ptr) ptr), cnt_(N) {}
            ~MemberArrayField() = default;

            bool IsMember () const override
            {
                return true;
            }
            bool IsConst () const override
            {
                return std::is_const_v<T>;
            }
            bool IsArray() const override
            {
                return true;
            }
            u32 Count() const override
            {
                return (u32)cnt_;
            }
            TypePtr Type () const override
            {
                return TypeOf<T> ();
            }

            Any Get (const Any *object, u32 index) const override
            {
                assert (index < cnt_);
                auto t_obj = (M_T *) object->ValuePtr<void> ();
                T* ptr = (t_obj->*ptr_) + index;
                return Any::RefWrap(ptr);
            }
            Any Set (const Any *object, Any value, u32 index) const override
            {
                assert (index < cnt_);
                auto t_obj = (M_T *) object->ValuePtr<void> ();
                if constexpr (std::is_const_v<T>)
                {
                    return Get (object, index);
                }
                else
                {
                    (t_obj->*ptr_)[index] = value.ValueRef<T> ();
                    return Get (object, index);
                }
            }
        private:
            Ptr          ptr_;
            size_t const cnt_;
        };

        template <typename T>
        FieldPtr MakeField (sview name, T *ptr)
        {
            return new NormalField {name, ptr};
        }

        template <typename C, typename T>
        FieldPtr MakeField (sview name, T (C::*ptr))
        {
#ifdef EXPLICIT_CLASS_NAME
            return new MemberField<C, T> {name, ptr};
#else
            return new MemberField<T> {name, ptr};
#endif
        }

        template <typename T, size_t N>
        FieldPtr MakeField(sview name, T(*ptr)[N])
        {
            return new NormalArrayField{name, ptr};
        }

        template <typename T, typename C, size_t N>
        FieldPtr MakeField(sview name, T(C::*ptr)[N])
        {
#ifdef EXPLICIT_CLASS_NAME
            return new MemberArrayField<C, T> {name, ptr};
#else
            return new MemberArrayField<T> {name, ptr};
#endif
        }
    }  // namespace details

    template <typename T>
    FieldPtr MakeField (sview name, T t)
    {
        return details::MakeField (name, t);
    }
}  // namespace Meta

#endif /* LIBMETA_FIELD_INL */
