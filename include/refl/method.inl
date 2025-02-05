#ifndef LIBMETA_METHOD_INL
#define LIBMETA_METHOD_INL

#include "refl/type.hpp"

namespace Meta
{
    namespace details
    {
        class UnExistsClass;
        using CLASS_TAG = UnExistsClass;
        // free method / static member
        template <typename R, typename... Param>
        class NormalMethod : public Method
        {
        public:
            NormalMethod (sview name, R (*ptr) (Param...)) : Method (name, TypeOf<R> ()), ptr_ (ptr)
            {
                assert (ptr_ != nullptr);
            }
            ~NormalMethod () override {}

            bool IsMember () const override
            {
                return false;
            }
            bool IsConst () const override
            {
                return false;
            }
            bool IsVolatile () const override
            {
                return false;
            }

            Any Invoke (Any *args, u32 cnt) const override
            {
                if (cnt < sizeof...(Param))
                    return {};
                else
                    return Apply (args, std::make_index_sequence<sizeof...(Param)> {});
            }

        private:
            template <size_t... Idx>
            Any Apply (Any *args, std::index_sequence<Idx...>) const
            {
                using TupleType = std::tuple<Param...>;
                if constexpr (std::is_same_v<void, R>)
                {
                    (*ptr_) (args[Idx].ValueRef<std::tuple_element_t<Idx, TupleType>> ()...);
                    return {};
                }
                else
                {
                    return (*ptr_) (args[Idx].ValueRef<std::tuple_element_t<Idx, TupleType>> ()...);
                }
            }

            R (*ptr_) (Param...);
        };

        template <typename R, typename... Param>
        class MemberMethodNormal : public Method
        {
            using Ptr = R (CLASS_TAG::*) (Param...);

        public:
            template <typename C>
            MemberMethodNormal (sview name, R (C::*ptr) (Param...)) : Method (name, TypeOf<R> ()), ptr_ ((Ptr) ptr)
            {
                assert (ptr_ != nullptr);
            }
            ~MemberMethodNormal () {}

            bool IsMember () const override
            {
                return true;
            }
            bool IsConst () const override
            {
                return false;
            }
            bool IsVolatile () const override
            {
                return false;
            }

            Any Invoke (Any *args, u32 cnt) const override
            {
                if (cnt < sizeof...(Param) + 1)
                    return {};
                // args[0] is class instance
                void *obj = args[0].ValuePtr<void>();
                return Apply(obj, args+1, std::make_index_sequence<sizeof...(Param)>{});
            }
        private:
            template <size_t ... Idx>
            Any Apply(void *obj, Any *args, std::index_sequence<Idx...>) const
            {
                using TupleType = std::tuple<Param...>;
                CLASS_TAG *t_obj = (CLASS_TAG*)obj;
                if constexpr (std::is_same_v<void, R>)
                {
                    (t_obj->*ptr_)(args[Idx].ValueRef<std::tuple_element_t<Idx, TupleType>>()...);
                    return {};
                }
                else
                {
                    return (t_obj->*ptr_)(args[Idx].ValueRef<std::tuple_element_t<Idx, TupleType>>()...);
                }
            }
            Ptr ptr_;
        };

        template <typename R, typename... Param>
        class MemberMethodConst : public Method
        {
            using Ptr = R (CLASS_TAG::*) (Param...) const;

        public:
            template <typename C>
            MemberMethodConst (sview name, R (C::*ptr) (Param...) const) : Method (name, TypeOf<R> ()), ptr_ ((Ptr) ptr)
            {
                assert (ptr_ != nullptr);
            }
            ~MemberMethodConst () {}

            bool IsMember () const override
            {
                return true;
            }
            bool IsConst () const override
            {
                return true;
            }
            bool IsVolatile () const override
            {
                return false;
            }

            Any Invoke (Any *args, u32 cnt) const override
            {
                if (cnt < sizeof...(Param) + 1)
                    return {};
                // args[0] is class instance
                void *obj = args[0].ValuePtr<void>();
                return Apply(obj, args+1, std::make_index_sequence<sizeof...(Param)>{});
            }
        private:
            template <size_t ... Idx>
            Any Apply(void *obj, Any *args, std::index_sequence<Idx...>) const
            {
                using TupleType = std::tuple<Param...>;
                CLASS_TAG *t_obj = (CLASS_TAG*)obj;
                if constexpr (std::is_same_v<void, R>)
                {
                    (t_obj->*ptr_)(args[Idx].ValueRef<std::tuple_element_t<Idx, TupleType>>()...);
                    return {};
                }
                else
                {
                    return (t_obj->*ptr_)(args[Idx].ValueRef<std::tuple_element_t<Idx, TupleType>>()...);
                }
            }
            Ptr ptr_;
        };

        template <typename R, typename... Param>
        class MemberMethodVolatile : public Method
        {
            using Ptr = R (CLASS_TAG::*) (Param...) volatile;

        public:
            template <typename C>
            MemberMethodVolatile (sview name, R (C::*ptr) (Param...) volatile) : Method (name, TypeOf<R> ()), ptr_ ((Ptr) ptr)
            {
                assert (ptr_ != nullptr);
            }
            ~MemberMethodVolatile () {}

            bool IsMember () const override
            {
                return true;
            }
            bool IsConst () const override
            {
                return false;
            }
            bool IsVolatile () const override
            {
                return true;
            }

            Any Invoke (Any *args, u32 cnt) const override
            {
                if (cnt < sizeof...(Param) + 1)
                    return {};
                // args[0] is class instance
                void *obj = args[0].ValuePtr<void>();
                return Apply(obj, args+1, std::make_index_sequence<sizeof...(Param)>{});
            }
        private:
            template <size_t ... Idx>
            Any Apply(void *obj, Any *args, std::index_sequence<Idx...>) const
            {
                using TupleType = std::tuple<Param...>;
                CLASS_TAG *t_obj = (CLASS_TAG*)obj;
                if constexpr (std::is_same_v<void, R>)
                {
                    (t_obj->*ptr_)(args[Idx].ValueRef<std::tuple_element_t<Idx, TupleType>>()...);
                    return {};
                }
                else
                {
                    return (t_obj->*ptr_)(args[Idx].ValueRef<std::tuple_element_t<Idx, TupleType>>()...);
                }
            }
            Ptr ptr_;
        };

        template <typename R, typename... Param>
        MethodPtr MakeMethod (sview name, R (*ptr) (Param...))
        {
            return MethodPtr {new NormalMethod {name, ptr}};
        };

        template <typename R, typename C, typename... Param>
        MethodPtr MakeMethod (sview name, R (C::*ptr) (Param...))
        {
            return MethodPtr {new MemberMethodNormal {name, ptr}};
        };

        template <typename R, typename C, typename... Param>
        MethodPtr MakeMethod (sview name, R (C::*ptr) (Param...) const)
        {
            return MethodPtr {new MemberMethodConst {name, ptr}};
        };

        template <typename R, typename C, typename... Param>
        MethodPtr MakeMethod (sview name, R (C::*ptr) (Param...) volatile)
        {
            return MethodPtr {new MemberMethodVolatile {name, ptr}};
        };

    }  // namespace details

    template <typename T>
    MethodPtr MakeMethod (sview name, T t)
    {
        return details::MakeMethod (name, t);
    }
}  // namespace Meta

#endif /* LIBMETA_METHOD_INL */
