#ifndef LIBMETA_TYPE_FWD_HPP
#define LIBMETA_TYPE_FWD_HPP

#include "refl/fwd.hpp"

namespace Meta
{
    str LIBMETA_API  AnyToString (const TypePtr &type, const Any &any);

    template <typename T>
    void AnyFromValue (const TypePtr &type, T&& value, Any &any);
}  // namespace Meta

#include <type_traits>

#define FUNDAMENTAL_TYPES(X) \
    X (s8);                  \
    X (s16);                 \
    X (s32);                 \
    X (s64);                 \
    X (u8);                  \
    X (u16);                 \
    X (u32);                 \
    X (u64);                 \
    X (f32);                 \
    X (f64);                 \
    X (bool);                \
    X (char)

#define INT_TYPES(X) \
    X (s8);          \
    X (s16);         \
    X (s32);         \
    X (s64);         \
    X (u8);          \
    X (u16);         \
    X (u32);         \
    X (u64)

#define LINKAGE_TYPEID(type) \
    template <>              \
    TypeId LIBMETA_API details::GetTypeIdImpl<type> ()

namespace Meta
{
    FUNDAMENTAL_TYPES (LINKAGE_TYPEID);

    // UserType
    LINKAGE_TYPEID(Type);
    // FieldType
    LINKAGE_TYPEID(Field);
    // MethodType
    LINKAGE_TYPEID(Method);
    // DelegateType
    LINKAGE_TYPEID(Delegate);
    // ConstantType
    LINKAGE_TYPEID(Constant);
    // void type
    LINKAGE_TYPEID(void);
    // str
    LINKAGE_TYPEID(str);
    // const char *
    template <>
    TypeId LIBMETA_API GetTypeId<cstr> ();
}

#undef LINKAGE_TYPEID

#endif /* LIBMETA_TYPE_FWD_HPP */
