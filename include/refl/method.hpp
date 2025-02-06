#ifndef LIBMETA_METHOD_HPP
#define LIBMETA_METHOD_HPP

#include "exports.h"
#include "refl/fwd.hpp"
#include "refl/method.hpp"
#include "refl/any.hpp"

#include <vector>
namespace Meta {
    struct ParameterInfo {
        sview   name;
        TypePtr type;
        Any def;
    };

    using Params = std::vector<ParameterInfo>;

    class LIBMETA_API Method {
        friend class MethodBuilder;
    public:
        Method(sview name, TypePtr rtype);
        virtual ~Method();

        [[nodiscard]] inline sview Name() const { return name_; }
        [[nodiscard]] inline sview Description() const { return desc_; }

        TypePtr ResultType() const;
        u32     ParameterCount() const;
        TypePtr ParameterType(u32 index) const;
        sview   ParameterName(u32 index) const;
        Any ParameterDefault(u32 index) const;

        virtual bool IsMember() const = 0;
        virtual bool IsConst() const = 0;
        virtual bool IsVolatile() const = 0;

        virtual Any Invoke(Any *args, u32 cnt) const = 0;

        template<typename ... Args>
        Any Invoke(Args ... args) const {
            Any va_arr[] = { args..., {} };
            return Invoke(va_arr, (u32)std::size(va_arr) - 1);
        }

    private:
        void AddParam(sview name, TypePtr type, Any def);

    private:
        sview    name_;
        sview    desc_;
        TypePtr  result_type_;
        Params   params_;
    };
}

#include "method.inl"

#endif /* LIBMETA_METHOD_HPP */
