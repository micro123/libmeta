#ifndef LIBMETA_JSONIFY_HPP
#define LIBMETA_JSONIFY_HPP

#include "refl/fwd.hpp"
#include <string>


namespace Meta
{
    class Any;

    LIBMETA_API
    str JsonSerialize(const Any& value, bool formatted = false);

    LIBMETA_API
    bool JsonDeserialize(const Any &obj, const str& value);
}


#endif  // LIBMETA_JSONIFY_HPP
