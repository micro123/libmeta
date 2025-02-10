#include "refl/generic_type.hpp"
#include <algorithm>
#include <iterator>
#include <refl/constant.hpp>
#include <refl/field.hpp>
#include <refl/method.hpp>

void Meta::GenericType::AddField (FieldPtr ptr)
{
    const str name = ptr->Name ().data ();
    fields_[name] = std::move (ptr);
}
void Meta::GenericType::AddMethod (MethodPtr ptr)
{
    const str name = ptr->Name ().data ();
    methods_[name] = std::move (ptr);
}
void Meta::GenericType::AddConstant (ConstantPtr ptr)
{
    const str name = ptr->Name ().data ();
    constants_[name] = std::move (ptr);
}

void Meta::GenericType::AddBaseClass (TypePtr ptr) {
    
}

std::vector<Meta::FieldPtr> Meta::GenericType::GetFields () const
{
    std::vector<FieldPtr> ret;
    ret.reserve(fields_.size ());
    std::ranges::transform (fields_, std::back_inserter (ret), [](const auto& f) {
        return f.second;
    });
    return ret;
}

Meta::FieldPtr Meta::GenericType::GetField (sview name)
{
    const auto it = fields_.find (name.data ());
    if (it == fields_.end ())
        return {};
    return it->second;
}

std::vector<Meta::MethodPtr> Meta::GenericType::GetMethods () const
{
    std::vector<MethodPtr> ret;
    ret.reserve(methods_.size ());
    std::ranges::transform (methods_, std::back_inserter (ret), [](const auto& f) {
        return f.second;
    });
    return ret;
}

Meta::MethodPtr Meta::GenericType::GetMethod (sview name) const
{
    const auto it = methods_.find (name.data ());
    if (it == methods_.end ())
        return {};
    return it->second;
}

std::vector<Meta::ConstantPtr> Meta::GenericType::GetConstants () const
{
    std::vector<ConstantPtr> ret;
    ret.reserve(constants_.size ());
    std::ranges::transform (constants_, std::back_inserter (ret), [](const auto& f) {
        return f.second;
    });
    return ret;
}

Meta::ConstantPtr Meta::GenericType::GetConstant (sview name) const
{
    const auto it = constants_.find (name.data ());
    if (it == constants_.end ())
        return {};
    return it->second;
}
