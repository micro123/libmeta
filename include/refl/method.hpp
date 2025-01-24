#ifndef LIBMETA_METHOD_HPP
#define LIBMETA_METHOD_HPP

#include "exports.h"
#include "refl/fwd.hpp"
#include "refl/method.hpp"
#include "utility/variant.hpp"

#include <vector>
namespace Meta {
    struct ParameterInfo {
        sview   name;
        TypePtr type;
        Variant def;
    };

    using Params = std::vector<ParameterInfo>;

    class LIBMETA_API Method {
    public:
        Method(sview name, TypePtr rtype);
        virtual ~Method();

        inline sview Name() const { return name_; }

        TypePtr ResultType() const;
        u32     ParameterCount() const;
        TypePtr ParameterType(u32 index) const;
        sview   ParameterName(u32 index) const;
        Variant ParameterDefault(u32 index) const;

        virtual bool IsMember() const = 0;
        virtual bool IsConst() const = 0;
        virtual bool IsVolatile() const = 0;

        virtual Variant Invoke(const Variant *args, u32 cnt) const = 0;

        template<typename ... Args>
        Variant Invoke(Args ... args) {
            const Variant va_arr[] = { Variant{} }; // TODO: sizeof...(args) + 1
            return Invoke(va_arr, (u32)std::size(va_arr));
        }

    protected:
        void AddParam(sview name, TypePtr type, Variant def);

    private:
        sview    name_;
        TypePtr  result_type_;
        Params   params_;
    };
}

#include "method.inl"

#endif /* LIBMETA_METHOD_HPP */
