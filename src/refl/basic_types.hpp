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

        bool FromString(const Any &obj, const str &content) const override {
            assert (obj.Type() == this);
            try {
                if constexpr (std::is_signed_v<T>)
                {
                    obj.ValueRef<T>() = static_cast<T>(std::stoll(content, nullptr, 0));
                }
                else if constexpr (std::is_signed_v<T>)
                {
                    obj.ValueRef<T>() = static_cast<T>(std::stoull(content, nullptr, 0));
                }
                else if constexpr (std::is_signed_v<T>)
                {
                    obj.ValueRef<T>() = static_cast<T>(std::stod(content, nullptr));
                }
            } catch (std::exception &) {
                return false;
            }
            return true;
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

        bool FromString(const Any &obj, const str &content) const override {
            assert (obj.Type() == (const Type*)this);
            obj.ValueRef<str>().assign(content);
            return true;
        }
    };

    class NullType final : public Type {
    public:
        NullType();
    };

    void RegisterFundamentalTypes(Registry *reg);
}

#endif /* LIBMETA_BASIC_TYPES_HPP */
