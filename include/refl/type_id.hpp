#ifndef LIBMETA_TYPE_ID_HPP
#define LIBMETA_TYPE_ID_HPP

#include "refl/fwd.hpp"
#include "utility/constexpr.hpp"

namespace Meta
{
    namespace details {
        template <typename T>
        TypeId GetTypeIdImpl ();

        template <typename T>
        TypeId GetTypeIdImpl ()
        {
            static StringName store(GetTypeName<T>());
            return store;
        }

        template <typename T>
        struct NoPtr
        {
            using type = T;
        };

        template <typename T>
        struct NoPtr<T*>
        {
            using type = T;
        };

        template <typename T>
        struct NoRef
        {
            using type = T;
        };

        template <typename T>
        struct NoRef<T&>
        {
            using type = T;
        };

        template <typename T>
        struct NoRef<Ref<T>>
        {
            using type = T;
        };

        template <typename T>
        struct NoConst
        {
            using type = T;
        };

        template <typename T>
        struct NoConst<const T>
        {
            using type = T;
        };

        template <typename T>
        struct NoVolatile
        {
            using type = T;
        };

        template <typename T>
        struct NoVolatile<volatile T>
        {
            using type = T;
        };
    }

    template <typename T>
    TypeId GetTypeId ()
    {
        using no_pointer_t = typename details::NoPtr<T>::type;
        using no_cvref_t = typename details::NoConst<typename details::NoVolatile<typename details::NoRef<no_pointer_t>::type>::type>::type;
        return details::GetTypeIdImpl<no_cvref_t>();
    }

    LIBMETA_API
    TypePtr TypeOf (const TypeId& id);

    template <typename T>
    TypePtr TypeOf ()
    {
        return TypeOf (GetTypeId<T> ());
    }

    inline TypePtr NilType() { return TypeOf(NULL_TYPE_ID); }

    template <typename T>
    constexpr u32 CalcTypeFlags ();
}

#endif /* LIBMETA_TYPE_ID_HPP */
