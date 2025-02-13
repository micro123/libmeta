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
                std::cout << std::format("      Parameter #{}: {} with type {}\n", i+1, x->ParameterName(i), x->ParameterType(i)->Name());
            }
        }
    }
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
    return 0;
}
