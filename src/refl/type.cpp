#include "refl/type.hpp"
#include <format>
#include <refl/any.hpp>
#include "refl/registry.hpp"

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

Meta::ConstantPtr              Meta::Type::GetConstant (sview name) const
{
    return {};
}

#define LINKAGE_TYPEID(type)                         \
    template <>                                      \
    TypeId details::GetTypeId<type> ()                        \
    {                                                \
        static u8 internal {1};                      \
        return reinterpret_cast<TypeId> (&internal); \
    }

namespace Meta
{
    FUNDAMENTAL_TYPES (LINKAGE_TYPEID);

    LINKAGE_TYPEID (void);
    LINKAGE_TYPEID (cstr);
    LINKAGE_TYPEID (str);

}  // namespace Meta

Meta::TypePtr Meta::TypeOf (Meta::TypeId tid)
{
    return Meta::Registry::Instance ().Get (tid);
}

std::vector<Meta::TypePtr> Meta::Type::GetBaseClasses () const
{
    return {};
}

std::string                Meta::Type::ToString (const Any &obj) const
{
    return std::format("{}(@{})", name_, obj.ValuePtr<void> ());
}
