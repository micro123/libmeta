#include "refl/field.hpp"
#include <format>

Meta::Field::Field (sview name): name_(name) {}

Meta::Field::~Field() = default;

bool Meta::Field::IsArray () const
{
    return false; // default is not an array
}

u32 Meta::Field::Count () const
{
    return 1; // default have one element
}

Meta::str Meta::Field::ToString() const
{
    return std::format("{}", name_);
}
