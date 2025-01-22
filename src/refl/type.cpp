#include "refl/type.hpp"
#include "refl/registry.hpp"

Meta::Type::Type(sview name, size_t size, u32 flags): name_(name), size_(size), flags_(flags)
{

}

Meta::Type::~Type() {}

#define LINKAGE_TYPEID(type) \
template<> TypeId GetTypeId<type>() { static u8 internal{1}; return reinterpret_cast<TypeId>(&internal); }

namespace Meta {
    FUNDAMENTAL_TYPES(LINKAGE_TYPEID);
}

Meta::Type *Meta::TypeOf(Meta::TypeId tid) {
    return Meta::Registry::Instance().Get(tid);
}
