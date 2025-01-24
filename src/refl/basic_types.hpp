#ifndef LIBMETA_BASIC_TYPES_HPP
#define LIBMETA_BASIC_TYPES_HPP

#include "refl/type.hpp"

namespace Meta::details {
    template <typename T>
    class FundamentalType final : public Type {
    public:
        FundamentalType(sview name): Type(name, sizeof(T), CalcTypeFlags<T>()) {}
    };

    class VoidType final: public Type {
    public:
        VoidType();
    };

    void RegisterFundamentalTypes(Registry *reg);
}

#endif /* LIBMETA_BASIC_TYPES_HPP */
