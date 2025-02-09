#include "serialization/jsonify.hpp"
#include <refl/any.hpp>

std::string Meta::JsonSerialize (const Meta::Any &value)
{
    return value.Value<std::string> ();
}

Meta::Any         Meta::JsonDeserialize (const std::string &value)
{
    return Meta::Any ();
}
