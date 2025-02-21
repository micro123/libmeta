#include "refl/property_container.hpp"

Meta::PropertyContainer::PropertyContainer () {}

Meta::Any Meta::PropertyContainer::GetProperty (str key) const
{
    auto it = properties_.find (key);
    if (it == end (properties_))
        return {};
    return it->second;
}

Meta::Any Meta::PropertyContainer::SetProperty (str key, Any value)
{
    return GetProperty (key) = value;
}

void Meta::PropertyContainer::AppendProperty (str key, Any value)
{
    properties_[key] = value;
}
