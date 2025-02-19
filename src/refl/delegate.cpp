#include "refl/delegate.hpp"
#include <format>

Meta::Delegate::Delegate (MethodPtr m) : Method ("anonymous", m->ResultType (), 0), mehtod_ (m)
{
    prefilled_args_.reserve (m->ParameterCount () + IsMember () ? 1 : 0);
}
Meta::Delegate::~Delegate () = default;

bool Meta::Delegate::IsConst () const
{
    return false;
}

bool Meta::Delegate::IsMember () const
{
    return mehtod_->IsMember ();
}

bool Meta::Delegate::IsVolatile () const
{
    return false;
}

void Meta::Delegate::AppendArg (const Any &any)
{
    if (IsMember ())
        assert (prefilled_args_.size () <= mehtod_->ParameterCount ());
    else
        assert (prefilled_args_.size () + 1 <= mehtod_->ParameterCount ());
    prefilled_args_.emplace_back (any);
}

Meta::str Meta::Delegate::ToString () const
{
    return std::format ("Delegate({})", mehtod_->Name ());
}

Meta::Any Meta::Delegate::InvokeWithArgs (Any *args, u32 cnt) const
{
    auto const total = (IsMember () ? 1 : 0) + mehtod_->ParameterCount ();
    if (!mehtod_->ParameterCountCheck(total, cnt))
        return {};
    std::vector<Any> vec_arg (total);
    vec_arg.assign (begin (prefilled_args_), end (prefilled_args_));
    for (u32 i = 0; i < cnt; ++i)
    {
        vec_arg.emplace_back (args[i]);
    }
    return mehtod_->InvokeWithArgs (vec_arg.data (), total);
}

u32 Meta::Delegate::ParameterCount () const
{
    return mehtod_->ParameterCount () - prefilled_args_.size ();
}
Meta::TypePtr Meta::Delegate::ParameterType (u32 index) const
{
    assert (index < ParameterCount ());
    return mehtod_->ParameterType (index + prefilled_args_.size ());
}
Meta::sview Meta::Delegate::ParameterName (u32 index) const
{
    assert (index < ParameterCount ());
    return mehtod_->ParameterName (index + prefilled_args_.size ());
}
Meta::Any Meta::Delegate::ParameterDefault (u32 index) const
{
    assert (index < ParameterCount ());
    return mehtod_->ParameterDefault (index + prefilled_args_.size ());
}
