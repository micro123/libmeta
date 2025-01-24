#ifndef LIBMETA_METHOD_INL
#define LIBMETA_METHOD_INL

#include "refl/type.hpp"

namespace Meta {
    namespace details {
        class UnExistsClass;
        using CLASS_TAG = UnExistsClass;
        // free method / static member
        template <typename R, typename ... Param>
        class NormalMethod : public Method {
        public:
            NormalMethod(sview name, R(*ptr)(Param...)): Method(name, TypeOf<R>()), ptr_(ptr) {
                assert(ptr_ != nullptr);
            }
            ~NormalMethod() override {}

            bool IsMember() const override { return false; }
            bool IsConst() const override { return false; }
            bool IsVolatile() const override { return false; }

            Variant Invoke(const Variant *args, u32 cnt) const override { return {}; } // TODO: Impl
        private:
            R(*ptr_)(Param...);
        };

        template <typename R, typename ... Param>
        class MemberMethodNormal : public Method {
            using Ptr = R(CLASS_TAG::*)(Param...);
        public:
            template <typename C>
            MemberMethodNormal(sview name, R(C::*ptr)(Param...)): Method(name, TypeOf<R>()), ptr_((Ptr)ptr) {
                assert(ptr_ != nullptr);
            }
            ~MemberMethodNormal() {

            }

            bool IsMember() const override { return true; }
            bool IsConst() const override { return false; }
            bool IsVolatile() const override { return false; }

            Variant Invoke(const Variant *args, u32 cnt) const override { return {}; } // TODO: Impl
        private:
            Ptr ptr_;
        };

        template <typename R, typename ... Param>
        class MemberMethodConst : public Method {
            using Ptr = R(CLASS_TAG::*)(Param...) const;
        public:
            template <typename C>
            MemberMethodConst(sview name, R(C::*ptr)(Param...) const): Method(name, TypeOf<R>()), ptr_((Ptr)ptr) {
                assert(ptr_ != nullptr);
            }
            ~MemberMethodConst() {

            }

            bool IsMember() const override { return true; }
            bool IsConst() const override { return true; }
            bool IsVolatile() const override { return false; }

            Variant Invoke(const Variant *args, u32 cnt) const override { return {}; } // TODO: Impl
        private:
            Ptr ptr_;
        };

        template <typename R, typename ... Param>
        class MemberMethodVolatile : public Method {
            using Ptr = R(CLASS_TAG::*)(Param...) volatile;
        public:
            template <typename C>
            MemberMethodVolatile(sview name, R(C::*ptr)(Param...) volatile): Method(name, TypeOf<R>()), ptr_((Ptr)ptr) {
                assert(ptr_ != nullptr);
            }
            ~MemberMethodVolatile() {

            }

            bool IsMember() const override { return true; }
            bool IsConst() const override { return false; }
            bool IsVolatile() const override { return true; }

            Variant Invoke(const Variant *args, u32 cnt) const override { return {}; } // TODO: Impl
        private:
            Ptr ptr_;
        };

        template <typename R, typename ... Param>
        MethodPtr MakeMethod(sview name, R(*ptr)(Param...)) {
            return MethodPtr{new NormalMethod{name, ptr}};
        };

        template <typename R, typename C, typename ... Param>
        MethodPtr MakeMethod(sview name, R(C::*ptr)(Param...)) {
            return MethodPtr{new MemberMethodNormal{name, ptr}};
        };

        template <typename R, typename C, typename ... Param>
        MethodPtr MakeMethod(sview name, R(C::*ptr)(Param...) const) {
            return MethodPtr{new MemberMethodConst{name, ptr}};
        };

        template <typename R, typename C, typename ... Param>
        MethodPtr MakeMethod(sview name, R(C::*ptr)(Param...) volatile) {
            return MethodPtr{new MemberMethodVolatile{name, ptr}};
        };

    }

    template <typename T>
    MethodPtr MakeMethod(sview name, T t) {
        return details::MakeMethod(name, t);
    }
}

#endif /* LIBMETA_METHOD_INL */
