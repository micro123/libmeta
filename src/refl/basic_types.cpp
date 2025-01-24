#include "basic_types.hpp"
#include "utility/constexpr.hpp"
#include "refl/registry.hpp"

Meta::details::VoidType::VoidType (): Type(GetTypeName<void>(), 0, 0) {}

#define REG_FUNDAMENTAL(t) \
do { \
    auto type = Meta::TypePtr{ new Meta::details::FundamentalType<t>(#t) };    \
    auto const ret = reg->RegisterType(std::move(type), Meta::GetTypeId<t>()); \
    assert (ret); \
} while (0)

void Meta::details::RegisterFundamentalTypes (Registry *reg) {
    FUNDAMENTAL_TYPES(REG_FUNDAMENTAL);
    auto ret = reg->RegisterType(new VoidType, GetTypeId<void>());
    assert(ret);
}
