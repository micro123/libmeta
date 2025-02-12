#include "refl/method.hpp"

Meta::Method::Method (sview name, TypePtr rtype) : name_ (name), result_type_ (std::move (rtype)) {}
Meta::Method::~Method () = default;

Meta::TypePtr Meta::Method::ResultType () const
{
    return result_type_;
}

u32 Meta::Method::ParameterCount () const
{
    return params_.size ();
}

Meta::TypePtr Meta::Method::ParameterType (u32 index) const
{
    assert (index < params_.size ());
    return TypeOf(params_.at (index).type_id);
}

Meta::sview Meta::Method::ParameterName (u32 index) const
{
    assert (index < params_.size ());
    return params_.at (index).name;
}

Meta::Any Meta::Method::ParameterDefault (u32 index) const
{
    assert (index < params_.size ());
    return params_.at (index).def;
}

void Meta::Method::AddParam (sview name, TypeId type_id, Any def) {
    params_.emplace_back(name, std::move(type_id), std::move(def));
}
