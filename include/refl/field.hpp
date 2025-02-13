#ifndef LIBMETA_FIELD_HPP
#define LIBMETA_FIELD_HPP

#include "refl/fwd.hpp"
#include "refl/any.hpp"

#include <string_view>

namespace Meta {
    class LIBMETA_API Field {
    public:
        Field(sview name);
        virtual ~Field();

        inline sview Name() const { return name_; }
        virtual TypePtr Type() const = 0;

        virtual bool IsMember() const = 0;
        virtual bool IsConst() const = 0;
        virtual bool IsArray() const;
        virtual u32  Count() const;

        virtual Any Get(const Any *object, u32 index = 0) const = 0;
        virtual Any Set(const Any *object, Any value, u32 index = 0) const = 0;
    private:
        sview const name_;
    };
}

#include "field.inl"

#endif /* LIBMETA_FIELD_HPP */
