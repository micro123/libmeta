#include "refl/method.hpp"
#include <sstream>

Meta::Method::Method (sview name, TypePtr rtype, u32 param_count) : name_ (name), result_type_ (std::move (rtype)) {
    params_.resize(param_count);
    default_param_start_ = ~u32(0);
}
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
    if (index < params_.size ())
        return TypeOf(params_.at (index).type_id);
    return {};
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

void Meta::Method::AddParamInfo (u32 idx, sview name, Any def) {
    assert (idx < params_.size ());
    auto &p = params_.at(idx);
    if (~default_param_start_ == 0 && def.IsValid())
        default_param_start_ = idx;
    p.def = std::move(def);
    p.name = std::move(name);
}

const Meta::Any *Meta::Method::ParameterOfDefault(Any *argv, u32 argn, u32 param_idx) const
{
    if (param_idx >= argn) {
        return &params_.at(param_idx).def;
    } else {
        return argv + param_idx;
    }
}

void Meta::Method::SetArgType (u32 idx, TypeId id) {
    assert (idx < params_.size ());
    auto &p = params_.at(idx);
    p.type_id = std::move(id);
}

void Meta::Method::SetArgTypes (TypeId *ids, u32 cnt) {
    for (u32 i=0; i<cnt; ++i) {
        SetArgType(i, ids[i]);
    }
}

bool Meta::Method::Verify() const {
    bool ok = true;
    if (~default_param_start_ != 0) // have default parameters
    {
        if (default_param_start_ >= params_.size())
        {
            ok = false; // too many arguments?
        }
        else
        {
            for (u32 i = default_param_start_; i<params_.size(); ++i)
            {
                if (!params_.at(i).def.IsValid())
                {
                    ok = false; // must all VALID from default_param_start_ to the end
                    break;
                }
            }
        }
    }
    return ok;
}

Meta::str Meta::Method::ToString () const
{
    // RType Name(P0_Type P0_Name,...) : desc
    std::ostringstream oss;
    oss << result_type_->Name() << ' ';
    oss << name_ << '(';
    auto const cnt = params_.size();
    auto i = 0u;
    for (auto &p : params_) {
        oss << p.name;
        if (++i < cnt)
            oss << ", ";
    }
    oss << ')';
    return oss.str();
}
