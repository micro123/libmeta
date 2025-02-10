#include "basic_types.hpp"
#include <string>
#include <cstring>
#include "refl/registry.hpp"
#include "utility/constexpr.hpp"
#include "refl/method.hpp"
#include "refl/field.hpp"


Meta::details::VoidType::VoidType () : Type (GetTypeName<void> (), 0, 0) {
    AddConversion<std::string>(
        +[](const Any &) -> Any {
            return std::string{"void"};
        }
    );
}

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
    auto ret = reg->RegisterType (new VoidType, Meta::GetTypeId<void> ());
    assert (ret);
    ret =reg->RegisterType(new CStringType, Meta::GetTypeId<cstr>());
    assert (ret);
    ret =reg->RegisterType(new StringType, Meta::GetTypeId<str>());
    assert (ret);
    ret = reg->RegisterType(new NullType, Meta::NULL_TYPE_ID);
    assert (ret);
}

Meta::details::CStringType::CStringType () : GenericType ("cstr", sizeof (cstr), CalcTypeFlags<cstr> ()) {
    AddConversion<std::string>(
        +[](const Any &obj) -> Any {
            return std::string{obj.ValueRef<cstr>()};
        }
    );
    AddMethod(MakeMethod("Length", +[](const char* str){ return strlen(str); }));
}

Meta::details::StringType::StringType () : GenericType ("str", sizeof (str), CalcTypeFlags<str> ()) {
    AddMethod(MakeMethod("Length", &std::string::length));
}

Meta::details::NullType::NullType () : Type("nil", 0, 0) {
AddConversion<std::string>(
        +[](const Any &) -> Any {
            return std::string{"nil"};
        }
    );
}
