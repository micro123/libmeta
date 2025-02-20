#include "refl/generic_type.hpp"
#include <algorithm>
#include <iterator>
#include <refl/constant.hpp>
#include <refl/field.hpp>
#include <refl/method.hpp>

static bool unique_check(auto const &container, const Meta::str &name) {
    bool ok = true;
    for (auto &x: container) {
        if (x.first == name) {
            ok = false;
            break;
        }
    }
    return ok;
}

void Meta::GenericType::AddField (FieldPtr ptr)
{
    const str name = ptr->Name ().data ();
    assert(unique_check(fields_, name));
    fields_.emplace_back(name, std::move(ptr));
}
void Meta::GenericType::AddMethod (MethodPtr ptr)
{
    const str name = ptr->Name ().data ();
    methods_.emplace_back(name, std::move(ptr));
}
void Meta::GenericType::AddConstant (ConstantPtr ptr)
{
    const str name = ptr->Name ().data ();
    assert(unique_check(constants_, name));
    constants_.emplace_back(name, std::move(ptr));
}

void Meta::GenericType::AddBaseClass (TypeId id, CastProc cast) {
    AddConversion(cast, id);
    base_classes_.emplace_back(id);
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
    const auto it = std::ranges::find_if(fields_, [name](auto& x) { return x.first == name; });
    if (it == fields_.end ())
    {
        // try base types
        std::list<FieldPtr> results;
        for (auto & base: GetBaseClasses()) {
            if (auto f = base->GetField(name))
                results.emplace_back(std::move(f));
        }
        if (results.size() == 1) // have many results are ambiguous
            return results.front();
    }
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
    const auto it = std::ranges::find_if(methods_, [name](auto& x) { return x.first == name; });
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
    const auto it = std::ranges::find_if(constants_, [name](auto& x) { return x.first == name; });
    if (it == constants_.end ())
    {
        // try base types
        std::list<ConstantPtr> results;
        for (auto & base: GetBaseClasses()) {
            if (auto c = base->GetConstant(name))
                results.emplace_back(std::move(c));
        }
        if (results.size() == 1) // have many results are ambiguous
            return results.front();
    }
    return it->second;
}

std::vector<Meta::TypePtr> Meta::GenericType::GetBaseClasses () const
{
    std::vector<TypePtr> result;
    for (auto const &x: base_classes_) {
        if (auto t = TypeOf(x))
            result.emplace_back(std::move(t));
    }
    return result;
}

std::vector<Meta::TypeId> Meta::GenericType::GetBaseTypeIds () const
{
    std::vector<TypeId> result;
    for (auto const &x: base_classes_) {
        result.emplace_back(x);
    }
    return result;
}
