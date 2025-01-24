#ifndef LIBMETA_FWD_HPP
#define LIBMETA_FWD_HPP

#include "exports.h"
#include "utility/ref.hpp"

#include <string_view>

#define FWDC(Type) \
    class Type;    \
    using Type##Ptr = Ref<Type>

namespace Meta
{
    FWDC (Type);
    FWDC (Method);
    FWDC (Field);
    class Variant;

    using TypeId = const void *;
    using sview  = std::string_view;
}  // namespace Meta

#undef FWDC

#endif /* LIBMETA_FWD_HPP */
