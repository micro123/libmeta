#include "refl/constant.hpp"
#include <utility>

Meta::Constant::Constant (sview name, Any value) : name_ (name), value_ (std::move (value)) {}
Meta::Constant::~Constant () = default;
Meta::sview Meta::Constant::Name () const
{
    return name_;
}
Meta::Any Meta::Constant::Value () const
{
    return value_;
}
