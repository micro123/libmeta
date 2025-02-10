#ifndef LIBMETA_FIELD_INL
#define LIBMETA_FIELD_INL
#include "builders.hpp"

namespace Meta
{
    namespace details
    {
        class UnExistsClass;
        using CLASS_TAG = UnExistsClass;

        // normal pointer
        template <typename T>
        class NormalField : public Field
        {
        public:
            NormalField (sview name, T *ptr) : Field (name), ptr_(ptr) {
                assert(ptr_ != nullptr);
            }
            ~NormalField () {}

            bool IsMember () const override { return false; }
            bool IsConst () const  override { return std::is_const_v<T>; }
            TypePtr Type() const override { return TypeOf<T>(); }

            Any Get (const Any*) const                override {
                // assert(Type() == object->Type());
                return *ptr_;
            }
            Any Set (const Any*, Any value) const override {
                // assert(Type() == object->Type());
                if constexpr (std::is_const_v<T>) {
                    return Get(nullptr);
                } else {
                    return *ptr_ = value.ValueRef<T>();
                }
            }

        private:
            T *ptr_;
        };

        template <typename T>
        class MemberField : public Field {
            using Ptr = T (CLASS_TAG::*);
        public:
            template <typename C>
            MemberField(sview name, T (C::*ptr)): Field(name)
            {
                memcpy(&ptr_, &ptr, sizeof(ptr_));
            }
            ~MemberField() override {}

            bool IsMember () const override { return true; }
            bool IsConst () const  override { return std::is_const_v<T>; }
            TypePtr Type() const override { return TypeOf<T>(); }

            Any Get (const Any *object) const                override {
                // assert(Type() == object->Type());
                auto t_obj = (CLASS_TAG*)object->ValuePtr<void>();
                return t_obj->*ptr_;
            }
            Any Set (const Any *object, Any value) const override {
                // assert(Type() == object->Type());
                auto t_obj = (CLASS_TAG*)object->ValuePtr<void>();
                if constexpr (std::is_const_v<T>) {
                    return Get(object);
                } else {
                    return t_obj->*ptr_ = value.ValueRef<T>();
                }
            }
        private:
            Ptr ptr_;
        };


        template <typename T>
        FieldPtr MakeField(sview name, T *ptr) {
            return new NormalField{name, ptr};
        }

        template <typename C, typename T>
        FieldPtr MakeField(sview name, T (C::*ptr)) {
            return new MemberField{name, ptr};
        }
    }  // namespace details

    template <typename T>
    FieldPtr MakeField (sview name, T t)
    {
        return details::MakeField(name, t);
    }
}  // namespace Meta

#endif /* LIBMETA_FIELD_INL */
