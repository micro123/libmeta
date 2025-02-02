#include "basic_types.hpp"
#include <string>
#include "refl/registry.hpp"
#include "utility/constexpr.hpp"

Meta::details::VoidType::VoidType () : Type (GetTypeName<void> (), 0, 0) {}

#define REG_FUNDAMENTAL(t)                                                             \
    do                                                                                 \
    {                                                                                  \
        auto       type = Meta::TypePtr {new Meta::details::FundamentalType<t> (#t)};  \
        auto const ret  = reg->RegisterType (std::move (type), Meta::GetTypeId<t> ()); \
        assert (ret);                                                                  \
    } while (0)

void Meta::details::RegisterFundamentalTypes (Registry *reg)
{
    FUNDAMENTAL_TYPES (REG_FUNDAMENTAL);
    auto ret = reg->RegisterType (new VoidType, GetTypeId<void> ());
    assert (ret);
    ret =reg->RegisterType(new CStringType, GetTypeId<cstr>());
    assert (ret);
    ret =reg->RegisterType(new StringType, GetTypeId<str>());
    assert (ret);
    ret = reg->RegisterType(new NullType, Meta::NULL_TYPE_ID);
    assert (ret);
}

Meta::details::CStringType::CStringType () : Type ("cstr", sizeof (cstr), CalcTypeFlags<cstr> ()) {}

Meta::details::StringType::StringType () : Type ("str", sizeof (str), CalcTypeFlags<str> ()) {}

Meta::details::NullType::NullType () : Type("nil", 0, 0) {}
