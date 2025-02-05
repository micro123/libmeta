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

        virtual bool IsMember() const = 0;
        virtual bool IsConst() const = 0;

        virtual Any Get(Any *object) const = 0;
        virtual Any Set(Any *object, Any value) const = 0;
    private:
        sview const name_;
    };
}

#include "field.inl"

#endif /* LIBMETA_FIELD_HPP */
