#ifndef LIBMETA_JSONIFY_HPP
#define LIBMETA_JSONIFY_HPP

#include <exports.h>
#include <string>


namespace Meta
{
    class Any;

    LIBMETA_API
    std::string JsonSerialize(const Any& value);

    LIBMETA_API
    Any JsonDeserialize(const std::string& value);
}


#endif  // LIBMETA_JSONIFY_HPP
