#ifndef LIBMETA_FWD_HPP
#define LIBMETA_FWD_HPP

#include "exports.h"
#include "refl/ref.hpp"
#include "utility/string_name.hpp"

#include <string_view>
#include <string>

#define FWDC(Type) \
    class Type;    \
    using Type##Ptr = Ref<Type>

namespace Meta
{
    namespace details{ class IView; }

    FWDC (Type);
    FWDC (Method);
    FWDC (Field);
    FWDC (Constant);
    FWDC (Delegate);
    class Any;
    class Registry;
    class TypeBuilder;

    using TypeId = StringName;
    using sview  = std::string_view;
    using cstr   = const char*;
    using str    = std::string;

    static_assert(sizeof(TypeId) == sizeof(void*));
    extern const LIBMETA_API TypeId NULL_TYPE_ID;

#ifndef EXPLICIT_CLASS_NAME
    namespace details {
        class UnExistsClass;
        using CLASS_TAG = UnExistsClass;
    }
#endif

#ifdef EXPLICIT_CLASS_NAME
#define M_T C
#else
#define M_T CLASS_TAG
#endif
}  // namespace Meta

#undef FWDC

#endif /* LIBMETA_FWD_HPP */
