#include "refl/constant.hpp"
#include <utility>
#include <format>

Meta::Constant::Constant (sview name, s64 value) : name_ (name), value_ (value) {}
Meta::Constant::~Constant () = default;
Meta::sview Meta::Constant::Name () const
{
    return name_;
}
Meta::Any Meta::Constant::Value () const
{
    return value_;
}
Meta::str Meta::Constant::ToString() const
{
    return std::format("{} = {}", name_, value_.Value<s64>());
}
