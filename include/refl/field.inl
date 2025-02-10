#ifndef LIBMETA_FIELD_INL
#define LIBMETA_FIELD_INL

namespace Meta
{
    namespace details
    {
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
                return *ptr_;
            }
            Any Set (const Any*, Any value) const override {
                if constexpr (std::is_const_v<T>) {
                    return Get(nullptr);
                } else {
                    return *ptr_ = value.ValueRef<T>();
                }
            }

        private:
            T *ptr_;
        };

        template <typename C, typename T>
        class MemberField : public Field {
            using Ptr = T (C::*);
        public:
            MemberField(sview name, T (C::*ptr)): Field(name), ptr_(ptr) {}
            ~MemberField() override {}

            bool IsMember () const override { return true; }
            bool IsConst () const  override { return std::is_const_v<T>; }
            TypePtr Type() const override { return TypeOf<T>(); }

            Any Get (const Any *object) const                override {
                return object->ValuePtr<C>()->*ptr_;
            }
            Any Set (const Any *object, Any value) const override {
                if constexpr (std::is_const_v<T>) {
                    return Get(object);
                } else {
                    return object->ValuePtr<C>()->*ptr_ = value.ValueRef<T>();
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
