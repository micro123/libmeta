#include "basic_types.hpp"
#include <string>
#include <cstring>
#include "refl/registry.hpp"
#include "utility/constexpr.hpp"
#include "refl/method.hpp"
#include "refl/delegate.hpp"
#include "refl/field.hpp"
#include "refl/constant.hpp"

Meta::details::VoidType::VoidType () : Type (GetTypeId<void>(), GetTypeName<void> (), 0, 0) {
    AddConversion<Meta::str>(
        +[](const Any &) -> Any {
            return Meta::str{"void"};
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
    ret = reg->RegisterType(new UserType, Meta::GetTypeId<Type>());
    assert (ret);
    ret = reg->RegisterType(new MethodType, Meta::GetTypeId<Method>());
    assert (ret);
    ret = reg->RegisterType(new ConstantType, Meta::GetTypeId<Constant>());
    assert (ret);
    ret = reg->RegisterType(new DelegateType, Meta::GetTypeId<Delegate>());
    assert (ret);
    ret = reg->RegisterType(new ViewType, Meta::GetTypeId<IView>());
    assert (ret);
}

Meta::details::CStringType::CStringType () : GenericType (GetTypeId<cstr>(), "cstr", sizeof (cstr), CalcTypeFlags<cstr> ()) {
    AddConversion<std::string>(
        +[](const Any &obj) -> Any {
            return Meta::str{obj.ValueRef<cstr>()};
        }
    );
    AddMethod(MakeMethod("Length", +[](const char* str){ return strlen(str); }));
}

Meta::details::StringType::StringType () : GenericType (GetTypeId<str>(), "str", sizeof (str), CalcTypeFlags<str> ()) {
    AddMethod(MakeMethod("Length", &Meta::str::length));
}

Meta::details::NullType::NullType () : Type(NULL_TYPE_ID, "nil", 0, 0) {
    AddConversion<Meta::str>(
        +[](const Any &) -> Any {
            return Meta::str{"nil"};
        }
    );
}

Meta::details::UserType::UserType (): GenericType (GetTypeId<Type>(), "type", sizeof(TypePtr), eTypeIsPtr | eTypeIsRef) {
    AddConversion<Meta::str>(
        +[](const Any &value) -> Any {
            return value.ValuePtr<Type>()->ToString();
        }
    );
}

Meta::details::MethodType::MethodType () : GenericType (GetTypeId<Method>(), "method", sizeof(MethodPtr), eTypeIsPtr | eTypeIsRef) {
    AddConversion<Meta::str>(
        +[](const Any &value) -> Any {
            return value.ValuePtr<Method>()->ToString();
        }
    );
}

Meta::details::ConstantType::ConstantType () : GenericType (GetTypeId<Constant>(), "constant", sizeof(ConstantPtr), eTypeIsPtr | eTypeIsRef) {
    AddConversion<Meta::str>(
        +[](const Any &value) -> Any {
            return value.ValuePtr<Constant>()->ToString();
        }
    );
}

Meta::details::FieldType::FieldType () : GenericType(GetTypeId<Field>(), "field", sizeof(FieldPtr), eTypeIsPtr | eTypeIsRef) {
    AddConversion<Meta::str>(
        +[](const Any &value) -> Any {
            return value.ValuePtr<Field>()->ToString();
        }
    );
}

Meta::details::DelegateType::DelegateType () : GenericType(GetTypeId<Delegate>(), "delegate", sizeof(DelegatePtr), eTypeIsPtr | eTypeIsRef) {
    AddConversion<Meta::str>(
        +[](const Any &value) -> Any {
            return value.ValuePtr<Delegate>()->ToString();
        }
    );
}

Meta::details::ViewType::ViewType () : GenericType(GetTypeId<IView>(), "view", sizeof(Ref<IView>), eTypeIsPtr | eTypeIsRef) {
    AddConversion<Meta::str>(
        +[](const Any &value) -> Any {
            return value.ValuePtr<IView>()->Get(GetTypeId<str>());
        }
    );
    AddConverter<Meta::str>(
        +[](const Any &out, const Any &in) -> bool {
            auto view = out.ValuePtr<IView>();
            return view->Set(in);
        }
    );
}
