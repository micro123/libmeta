#include "refl/type.hpp"
#include "refl/registry.hpp"

Meta::Type::Type (sview name, size_t size, u32 flags) : name_ (name), size_ (size), flags_ (flags) {}

Meta::Type::~Type () {}

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

Meta::MethodPtr Meta::Type::GetMethods (sview name) const
{
    return {};
}

#define LINKAGE_TYPEID(type)                         \
    template <>                                      \
    TypeId GetTypeId<type> ()                        \
    {                                                \
        static u8 internal {1};                      \
        return reinterpret_cast<TypeId> (&internal); \
    }

namespace Meta
{
    FUNDAMENTAL_TYPES (LINKAGE_TYPEID);

    LINKAGE_TYPEID(void);
}

Meta::TypePtr Meta::TypeOf (Meta::TypeId tid)
{
    return Meta::Registry::Instance ().Get (tid);
}
