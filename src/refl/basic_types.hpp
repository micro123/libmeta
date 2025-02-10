#ifndef LIBMETA_BASIC_TYPES_HPP
#define LIBMETA_BASIC_TYPES_HPP

#include "refl/type.hpp"
#include "refl/any.hpp"
#include "refl/generic_type.hpp"

namespace Meta::details {
    template <typename T>
    class FundamentalType final : public Type {
    public:
        FundamentalType(sview name): Type(name, sizeof(T), CalcTypeFlags<T>()) {
            AddConversion<std::string>(
                +[](const Any &obj) -> Any {
                    return std::to_string(obj.template ValueRef<T>());
                }
            );
        }
    };

    class VoidType final: public Type {
    public:
        VoidType();
    };

    class CStringType final : public GenericType {
    public:
        CStringType();
    };

    class StringType final :public GenericType {
    public:
        StringType();
    };

    class NullType final : public Type {
    public:
        NullType();
    };

    void RegisterFundamentalTypes(Registry *reg);
}

#endif /* LIBMETA_BASIC_TYPES_HPP */
