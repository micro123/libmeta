#ifndef LIBMETA_FWD_HPP
#define LIBMETA_FWD_HPP

#include "exports.h"
#include "refl/ref.hpp"

#include <string_view>
#include <string>

#define FWDC(Type) \
    class Type;    \
    using Type##Ptr = Ref<Type>

namespace Meta
{
    FWDC (Type);
    FWDC (Method);
    FWDC (Field);
    FWDC (Constant);
    class Any;
    class Registry;
    class TypeBuilder;

    using TypeId = uintptr_t;
    using sview  = std::string_view;
    using cstr   = const char*;
    using str    = std::string;

    static_assert(sizeof(TypeId) == sizeof(void*));
    constexpr TypeId NULL_TYPE_ID = (TypeId)0;

#ifndef EXPLICIT_CLASS_NAME
    namespace details {
        class UnExistsClass;
        using CLASS_TAG = UnExistsClass;
    }
#endif

#ifdef EXPLICIT_CLASS_NAME
#define M_T T
#else
#define M_T CLASS_TAG
#endif
}  // namespace Meta

#undef FWDC

#endif /* LIBMETA_FWD_HPP */
