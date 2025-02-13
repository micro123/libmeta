#include "refl/field.hpp"

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
