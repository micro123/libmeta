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

            Any Get (const Any *) const override
            {
                // assert(Type() == object->Type());
                return *ptr_;
            }
            Any Set (const Any *, Any value) const override
            {
                // assert(Type() == object->Type());
                if constexpr (std::is_const_v<T>)
                {
                    return Get (nullptr);
                }
                else
                {
                    return *ptr_ = value.ValueRef<T> ();
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

            Any Get (const Any *object) const override
            {
                // assert(Type() == object->Type());
                auto t_obj = (M_T*) object->ValuePtr<void> ();
                return t_obj->*ptr_;
            }
            Any Set (const Any *object, Any value) const override
            {
                // assert(Type() == object->Type());
                auto t_obj = (M_T *) object->ValuePtr<void> ();
                if constexpr (std::is_const_v<T>)
                {
                    return Get (object);
                }
                else
                {
                    return t_obj->*ptr_ = value.ValueRef<T> ();
                }
            }

        private:
            Ptr ptr_;
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
    }  // namespace details

    template <typename T>
    FieldPtr MakeField (sview name, T t)
    {
        return details::MakeField (name, t);
    }
}  // namespace Meta

#endif /* LIBMETA_FIELD_INL */
