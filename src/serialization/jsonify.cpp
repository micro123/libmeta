#include "serialization/jsonify.hpp"
#include <refl/any.hpp>
#include <refl/field.hpp>

#include <format>

std::string Meta::JsonSerialize (const Meta::Any &value)
{
    auto type = value.Type();
    if (!type)
        return "null";
    auto fields = type->GetFields();
    if (fields.empty())
        return std::format("\"{}\"", value.Value<std::string>());
    else {
        std::string result = "{";
        bool first = true;
        for (auto &x: fields) {
            if (x->IsMember()) {
                if (first)
                    first = false;
                else
                    result.append(",");
                result.append(std::format("\"{}\":{}", x->Name(), JsonSerialize(x->Get(&value))));
            }
        }
        result.append("}");
        return result;
    }
}

Meta::Any         Meta::JsonDeserialize (const std::string &value)
{
    return Meta::Any ();
}
