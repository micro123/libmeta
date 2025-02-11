#ifndef LIBMETA_METHOD_INL
#define LIBMETA_METHOD_INL

#include "refl/type.hpp"

namespace Meta
{
    namespace details
    {
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
                    return Any::Void;
                }
                else
                {
                    return (*ptr_) (args[Idx].ValueRef<std::tuple_element_t<Idx, TupleType>> ()...);
                }
            }

            R (*ptr_) (Param...);
        };

#ifdef EXPLICIT_CLASS_NAME
        template <typename R, typename C, typename... Param>
#else
        template <typename R, typename... Param>
#endif
        class MemberMethodNormal : public Method
        {
            using Ptr = R (M_T::*) (Param...);

        public:
#ifndef EXPLICIT_CLASS_NAME
            template <typename C>
#endif
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
                void *obj = args[0].ValuePtr<void> ();
                return Apply (obj, args + 1, std::make_index_sequence<sizeof...(Param)> {});
            }

        private:
            template <size_t... Idx>
            Any Apply (void *obj, Any *args, std::index_sequence<Idx...>) const
            {
                using TupleType = std::tuple<Param...>;
                M_T *t_obj      = (M_T *) obj;
                if constexpr (std::is_same_v<void, R>)
                {
                    (t_obj->*ptr_) (args[Idx].ValueRef<std::tuple_element_t<Idx, TupleType>> ()...);
                    return Any::Void;
                }
                else
                {
                    return (t_obj->*ptr_) (args[Idx].ValueRef<std::tuple_element_t<Idx, TupleType>> ()...);
                }
            }
            Ptr ptr_;
        };

#ifdef EXPLICIT_CLASS_NAME
        template <typename R, typename C, typename... Param>
#else
        template <typename R, typename... Param>
#endif
        class MemberMethodConst : public Method
        {
            using Ptr = R (M_T::*) (Param...) const;

        public:
#ifndef EXPLICIT_CLASS_NAME
            template <typename C>
#endif
            MemberMethodConst (sview name, R (C::*ptr) (Param...) const) : Method (name, TypeOf<R> ()), ptr_ ((Ptr ) ptr)
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
                void *obj = args[0].ValuePtr<void> ();
                return Apply (obj, args + 1, std::make_index_sequence<sizeof...(Param)> {});
            }

        private:
            template <size_t... Idx>
            Any Apply (void *obj, Any *args, std::index_sequence<Idx...>) const
            {
                using TupleType = std::tuple<Param...>;
                M_T *t_obj      = (M_T *) obj;
                if constexpr (std::is_same_v<void, R>)
                {
                    (t_obj->*ptr_) (args[Idx].ValueRef<std::tuple_element_t<Idx, TupleType>> ()...);
                    return Any::Void;
                }
                else
                {
                    return (t_obj->*ptr_) (args[Idx].ValueRef<std::tuple_element_t<Idx, TupleType>> ()...);
                }
            }
            Ptr ptr_;
        };

#ifdef EXPLICIT_CLASS_NAME
        template <typename R, typename C, typename... Param>
#else
        template <typename R, typename... Param>
#endif
        class MemberMethodVolatile : public Method
        {
            using Ptr = R (M_T::*) (Param...) volatile;

        public:
#ifndef EXPLICIT_CLASS_NAME
            template <typename C>
#endif
            MemberMethodVolatile (sview name, R (C::*ptr) (Param...) volatile) : Method (name, TypeOf<R> ()), ptr_ ((Ptr ) ptr)
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
                void *obj = args[0].ValuePtr<void> ();
                return Apply (obj, args + 1, std::make_index_sequence<sizeof...(Param)> {});
            }

        private:
            template <size_t... Idx>
            Any Apply (void *obj, Any *args, std::index_sequence<Idx...>) const
            {
                using TupleType = std::tuple<Param...>;
                M_T *t_obj      = (M_T *) obj;
                if constexpr (std::is_same_v<void, R>)
                {
                    (t_obj->*ptr_) (args[Idx].ValueRef<std::tuple_element_t<Idx, TupleType>> ()...);
                    return Any::Void;
                }
                else
                {
                    return (t_obj->*ptr_) (args[Idx].ValueRef<std::tuple_element_t<Idx, TupleType>> ()...);
                }
            }
            Ptr ptr_;
        };

        template <typename R, typename... Param>
        MethodPtr MakeMethod (sview name, R (*ptr) (Param...))
        {
            return new NormalMethod<R, Param...> {name, ptr};
        };

        template <typename R, typename C, typename... Param>
        MethodPtr MakeMethod (sview name, R (C::*ptr) (Param...))
        {
#ifdef EXPLICIT_CLASS_NAME
            return new MemberMethodNormal<R, C, Param...> {name, ptr};
#else
            return new MemberMethodNormal<R, Param...> {name, ptr};
#endif
        };

        template <typename R, typename C, typename... Param>
        MethodPtr MakeMethod (sview name, R (C::*ptr) (Param...) const)
        {
#ifdef EXPLICIT_CLASS_NAME
            return new MemberMethodConst<R, C, Param...> {name, ptr};
#else
            return new MemberMethodConst<R, Param...> {name, ptr};
#endif
        };

        template <typename R, typename C, typename... Param>
        MethodPtr MakeMethod (sview name, R (C::*ptr) (Param...) volatile)
        {
#ifdef EXPLICIT_CLASS_NAME
            return new MemberMethodVolatile<R, C, Param...> {name, ptr};
#else
            return new MemberMethodVolatile<R, Param...> {name, ptr};
#endif
        };

    }  // namespace details

    template <typename T>
    MethodPtr MakeMethod (sview name, T t)
    {
        return details::MakeMethod (name, t);
    }
}  // namespace Meta

#endif /* LIBMETA_METHOD_INL */
