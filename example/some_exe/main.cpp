#include <format>
#include <iostream>
#include <refl/registry.hpp>
#include <refl/type.hpp>
#include <refl/field.hpp>
#include <refl/method.hpp>
#include <refl/constant.hpp>

#include <serialization/jsonify.hpp>
#include "lib.hpp"

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
        std::cout << "  Fields:\n";
        for (auto &x: methods)
        {
            std::cout << std::format("    {}\n", x->Name());
        }
    }
}

int main(int argc, char const *argv[])
{
    Meta::Registry::Instance ().VisitTypes ([](Meta::TypeId tid, Meta::TypePtr type) -> bool {
        std::cout << std::format("type {} has id {}\n", type->Name (), tid.operator std::string());
        PrintType(type);
        return true;
    });

    Meta::Any test = MyClass();
    std::cout << Meta::JsonSerialize(test) << std::endl;
    return 0;
}
