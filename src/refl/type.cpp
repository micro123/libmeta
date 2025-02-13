#include "refl/type.hpp"
#include <format>
#include <refl/any.hpp>
#include "refl/registry.hpp"

const Meta::StringName Meta::NULL_TYPE_ID {"nil"};

Meta::Type::Type (sview name, size_t size, u32 flags) : name_ (name), size_ (size), flags_ (flags) {}

Meta::Type::~Type () = default;

std::vector<Meta::FieldPtr> Meta::Type::GetFields () const
{
    return {};
}

Meta::FieldPtr Meta::Type::GetField (sview name)
{
    return {};
}

std::vector<Meta::MethodPtr> Meta::Type::GetMethods () const
{
    return {};
}

Meta::MethodPtr Meta::Type::GetMethod (sview name) const
{
    return {};
}

std::vector<Meta::ConstantPtr> Meta::Type::GetConstants () const
{
    return {};
}

Meta::ConstantPtr Meta::Type::GetConstant (sview name) const
{
    return {};
}

#define LINKAGE_TYPEID(type)                \
    template <>                             \
    TypeId details::GetTypeIdImpl<type> ()  \
    {                                       \
        static StringName internal (#type); \
        return internal;                    \
    }

namespace Meta
{
    FUNDAMENTAL_TYPES (LINKAGE_TYPEID);

    LINKAGE_TYPEID (void);
    LINKAGE_TYPEID (str);

    template <>
    TypeId GetTypeId<cstr> ()
    {
        static StringName internal ("cstr");
        return internal;
    }

}  // namespace Meta

Meta::TypePtr Meta::TypeOf (Meta::TypeId tid)
{
    return Meta::Registry::Instance ().Get (tid);
}

std::vector<Meta::TypePtr> Meta::Type::GetBaseClasses () const
{
    return {};
}

std::string Meta::Type::ToString (const Any &obj) const
{
    auto const tid = GetTypeId<std::string> ();
    auto const it  = cast_ops_.find (tid);
    if (it == end (cast_ops_))
        return std::format ("{}(@{})", name_, obj.ValuePtr<void> ());
    return it->second (obj).Value<std::string> ();
}

bool Meta::Type::FromString (const Any &obj, const str &data) const
{
    return false;
}

void Meta::Type::AddConversion (CastPorc proc, TypeId type_id)
{
    cast_ops_[type_id] = proc;
}

bool Meta::Type::CanCast (TypeId dst) const
{
    return cast_ops_.find (dst) != end (cast_ops_);
}

bool Meta::Type::Cast (const Any &obj, Any &out, TypeId dst) const
{
    out = cast_ops_.at (dst) (obj);
    return out.IsValid ();
}
