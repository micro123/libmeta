#include "serialization/jsonify.hpp"
#include <refl/any.hpp>
#include <refl/field.hpp>

#include <nlohmann/json.hpp>

using namespace nlohmann;

static json ToJson(const Meta::Any &value)
{
    auto type = value.Type();
    if (!type)
        return json{nullptr};
    auto fields = type->GetFields();
    if (fields.empty())
        return value.Value<Meta::str>();
    else {
        nlohmann::json result = nlohmann::json::object();
        for (auto &x: fields) {
            if (x->IsMember()) {
                result[x->Name()] = ToJson(x->Get(&value));
            }
        }
        return result;
    }
}

Meta::str Meta::JsonSerialize (const Meta::Any &value, bool formated)
{
    return ToJson(value).dump(formated ? 2 : -1);
}

static bool FromJson(const Meta::Any& obj, const json &content)
{
    auto type = obj.Type();
    if (!type)
        return true;
    auto fields = type->GetFields();
    if (fields.empty()) {
        return type->FromString(obj, content.get<Meta::str>());
    }
    for (auto &x: fields) {
        if (x->IsMember()) {
            FromJson(x->Get(&obj), content[x->Name()]);
        }
    }
    return true;
}

bool Meta::JsonDeserialize(const Any& obj, const Meta::str& value)
{
    return FromJson(obj, json::parse(value));
}
