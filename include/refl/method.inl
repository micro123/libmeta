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
            NormalMethod (sview name, R (*ptr) (Param...)) : Method (name, GetTypeId<R> (), sizeof...(Param)), ptr_ (ptr)
            {
                assert (ptr_ != nullptr);
                SetArgTypes<Param...>();
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

            Any InvokeWithArgs (Any *args, u32 cnt) const override
            {
                // check passed arg count
                if (!ParameterCountCheck(cnt))
                    return {};
                else
                    return Apply (args, cnt, std::make_index_sequence<sizeof...(Param)> {});
            }

            TypePtr ResultType() const
            {
                return TypeOf<R>();
            }

        private:
            template <size_t... Idx>
            Any Apply (Any *args, u32 cnt, std::index_sequence<Idx...>) const
            {
                using TupleType = std::tuple<Param...>;
                if constexpr (std::is_same_v<void, R>)
                {
                    (*ptr_) (ParameterOfDefault(args, cnt, Idx)->Value<std::tuple_element_t<Idx, TupleType>> ()...);
                    return Any::Void;
                }
                else
                {
                    return (*ptr_) (ParameterOfDefault(args, cnt, Idx)->Value<std::tuple_element_t<Idx, TupleType>> ()...);
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
            MemberMethodNormal (sview name, R (C::*ptr) (Param...))
            : Method (name, GetTypeId<R> (), sizeof...(Param) + 1), ptr_ ((Ptr) ptr), cls_id_ (GetTypeId<C> ())
            {
                assert (ptr_ != nullptr);
                SetArgType (0, cls_id_);
                AddParamInfo (0, "this");
                SetArgTypes<Param...>();
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

            Any InvokeWithArgs (Any *args, u32 cnt) const override
            {
                // check passed arg count
                if (!ParameterCountCheck(cnt))
                    return {};
                // args[0] is class instance
                auto t_obj = GetObject(args);
                if (!t_obj)
                    return {};
                return Apply (t_obj, args + 1, cnt - 1, std::make_index_sequence<sizeof...(Param)> {});
            }

        private:
            template <size_t... Idx>
            Any Apply (M_T *t_obj, Any *args, u32 cnt, std::index_sequence<Idx...>) const
            {
                using TupleType = std::tuple<Param...>;
                if constexpr (std::is_same_v<void, R>)
                {
                    (t_obj->*ptr_) (ParameterOfDefault(args, cnt, Idx)->Value<std::tuple_element_t<Idx, TupleType>> ()...);
                    return Any::Void;
                }
                else
                {
                    return (t_obj->*ptr_) (ParameterOfDefault(args, cnt, Idx)->Value<std::tuple_element_t<Idx, TupleType>> ()...);
                }
            }
            inline M_T *GetObject (const Any *object) const
            {
                if (object->Id () == cls_id_)
                    return (M_T *) object->ValuePtr<void> ();
                else
                {
                    // try cast
                    Any cls_obj;
                    if (AnyCast (*object, cls_obj, cls_id_))
                        return (M_T *) cls_obj.ValuePtr<void> ();
                }
                return nullptr;
            }
            Ptr ptr_;
            TypeId const cls_id_;
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
            MemberMethodConst (sview name, R (C::*ptr) (Param...) const)
            : Method (name, GetTypeId<R> (), sizeof...(Param) + 1), ptr_ ((Ptr ) ptr), cls_id_ (GetTypeId<C> ())
            {
                assert (ptr_ != nullptr);
                SetArgType (0, cls_id_);
                AddParamInfo (0, "this");
                SetArgTypes<Param...>();
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

            Any InvokeWithArgs (Any *args, u32 cnt) const override
            {
                // check passed arg count
                if (!ParameterCountCheck(cnt))
                    return {};
                // args[0] is class instance
                auto t_obj = GetObject(args);
                if (!t_obj)
                    return {};
                return Apply (t_obj, args + 1, cnt - 1, std::make_index_sequence<sizeof...(Param)> {});
            }

        private:
            template <size_t... Idx>
            Any Apply (M_T *t_obj, Any *args, u32 cnt, std::index_sequence<Idx...>) const
            {
                using TupleType = std::tuple<Param...>;
                if constexpr (std::is_same_v<void, R>)
                {
                    (t_obj->*ptr_) (ParameterOfDefault(args, cnt, Idx)->Value<std::tuple_element_t<Idx, TupleType>> ()...);
                    return Any::Void;
                }
                else
                {
                    return (t_obj->*ptr_) (ParameterOfDefault(args, cnt, Idx)->Value<std::tuple_element_t<Idx, TupleType>> ()...);
                }
            }
            inline M_T *GetObject (const Any *object) const
            {
                if (object->Id () == cls_id_)
                    return (M_T *) object->ValuePtr<void> ();
                else
                {
                    // try cast
                    Any cls_obj;
                    if (AnyCast (*object, cls_obj, cls_id_))
                        return (M_T *) cls_obj.ValuePtr<void> ();
                }
                return nullptr;
            }
            Ptr ptr_;
            TypeId const cls_id_;
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
            MemberMethodVolatile (sview name, R (C::*ptr) (Param...) volatile)
            : Method (name, GetTypeId<R> (), sizeof...(Param) + 1), ptr_ ((Ptr ) ptr), cls_id_ (GetTypeId<C> ())
            {
                assert (ptr_ != nullptr);
                SetArgType (0, cls_id_);
                AddParamInfo (0, "this");
                SetArgTypes<Param...>();
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

            Any InvokeWithArgs (Any *args, u32 cnt) const override
            {
                // check passed arg count
                if (!ParameterCountCheck(cnt))
                    return {};
                // args[0] is class instance
                auto t_obj = GetObject(args);
                if (!t_obj)
                    return {};
                return Apply (t_obj, args + 1, cnt - 1, std::make_index_sequence<sizeof...(Param)> {});
            }

        private:
            template <size_t... Idx>
            Any Apply (M_T *t_obj, Any *args, u32 cnt, std::index_sequence<Idx...>) const
            {
                using TupleType = std::tuple<Param...>;
                if constexpr (std::is_same_v<void, R>)
                {
                    (t_obj->*ptr_) (ParameterOfDefault(args, cnt, Idx)->Value<std::tuple_element_t<Idx, TupleType>> ()...);
                    return Any::Void;
                }
                else
                {
                    return (t_obj->*ptr_) (ParameterOfDefault(args, cnt, Idx)->Value<std::tuple_element_t<Idx, TupleType>> ()...);
                }
            }
            inline M_T *GetObject (const Any *object) const
            {
                if (object->Id () == cls_id_)
                    return (M_T *) object->ValuePtr<void> ();
                else
                {
                    // try cast
                    Any cls_obj;
                    if (AnyCast (*object, cls_obj, cls_id_))
                        return (M_T *) cls_obj.ValuePtr<void> ();
                }
                return nullptr;
            }
            Ptr ptr_;
            TypeId const cls_id_;
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
