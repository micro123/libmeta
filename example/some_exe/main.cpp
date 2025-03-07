#include <format>
#include <iostream>
#include <refl/registry.hpp>
#include <refl/type.hpp>
#include <refl/field.hpp>
#include <refl/method.hpp>
#include <refl/constant.hpp>

#include <serialization/jsonify.hpp>
#include <serialization/xml.hpp>
#include "lib.hpp"
#include "custom_sections.hpp"

static std::string GetTypeName(Meta::TypeId id)
{
    auto t = TypeOf(id);
    if (t)
        return {t->Name ().data (), t->Name ().size ()};
    return "<unknown>";
}

static void PrintType(const Meta::TypePtr &ptr)
{
    auto fields = ptr->GetFields();
    if (!fields.empty())
    {
        std::cout << "  Fields:\n";
        for (auto &x: fields)
        {
            std::cout << std::format("    {}: {}\n", x->Name(), x->Type()->Name());
        }
    }

    auto consts = ptr->GetConstants();
    if (!consts.empty())
    {
        std::cout << "  Constants:\n";
        for (auto &x: consts)
        {
            std::cout << std::format("    {} = {}\n", x->Name(), x->Value().Value<s64>());
        }
    }

    auto methods = ptr->GetMethods();
    if (!methods.empty())
    {
        std::cout << "  Methods:\n";
        for (auto &x: methods)
        {
            std::cout << std::format("    {}\n", x->Name());
            auto const cnt = x->ParameterCount();
            for (auto i=0u; i<cnt; ++i)
            {
                std::cout << std::format("      Parameter #{}: {} with type {} and default value {}\n", i+1, x->ParameterName(i), GetTypeName (x->ParameterType(i)), x->ParameterDefault(i).Value<Meta::str>());
            }
        }
    }

    auto constructors = ptr->GetConstructors();
    if (!constructors.empty())
    {
        std::cout << "  Constructors:\n";
        for (auto &x: constructors)
        {
            std::cout << std::format("    {}\n", x->Name());
            auto const cnt = x->ParameterCount();
            for (auto i=0u; i<cnt; ++i)
            {
                std::cout << std::format("      Parameter #{}: {} with type {} and default value {}\n", i+1, x->ParameterName(i), GetTypeName (x->ParameterType(i)), x->ParameterDefault(i).Value<Meta::str>());
            }
        }
    }
}

static void test_ctor()
{
    using namespace Meta;
    auto cls = TypeOf<MyClass>();
    assert (cls != nullptr);
    Any obj;
    bool ok = cls->Instantiate (obj, "👋");
    assert (ok);
    std::cout << JsonSerialize(obj, true) << std::endl;
}

int main(int argc, char const *argv[])
{
    call_all(&__start_reg, &__stop_reg);

    Meta::Registry::Instance ().VisitTypes ([](Meta::TypeId tid, Meta::TypePtr type) -> bool {
        std::cout << std::format("type {} has id {}\n", type->Name (), tid.operator std::string());
        PrintType(type);
        return true;
    });

    Meta::Any test = MyClass("你好");
    std::cout << Meta::JsonSerialize(test,true) << std::endl;
    std::cout << Meta::XmlSerialize(test,true) << std::endl;
    std::cout << test["data"]["d"] << std::endl;
    std::cout << test["Test"](999) << std::endl;
    Meta::Any x = Meta::Any::NewRef<int>(42);
    Meta::Any y = Meta::Any::NewRef<f32>(3.14f);
    std::cout << test["Inc"](x, y) << "\n";
    std::cout << x << ' ' << y << std::endl;
    Meta::Any type = test.Type();
    std::cout << type["Inc"] << '\n';

    test_ctor();

    return 0;
}
