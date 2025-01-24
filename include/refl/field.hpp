#ifndef LIBMETA_FIELD_HPP
#define LIBMETA_FIELD_HPP

#include "refl/fwd.hpp"
#include "utility/variant.hpp"

#include <string_view>

namespace Meta {
    class LIBMETA_API Field {
    public:
        Field(sview name);
        virtual ~Field();

        inline sview Name() const { return name_; }

        virtual bool IsMember() const = 0;
        virtual bool IsConst() const = 0;

        virtual Variant Get(Variant object) const = 0;
        virtual Variant Set(Variant object, Variant value) const = 0;
    private:
        sview const name_;
    };
}

#include "refl/field.inl"

#endif /* LIBMETA_FIELD_HPP */
