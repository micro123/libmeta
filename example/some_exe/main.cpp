#include <format>
#include <iostream>
#include <refl/registry.hpp>
#include <refl/type.hpp>

int main(int argc, char const *argv[])
{
    Meta::Registry::Instance ().VisitTypes ([](Meta::TypeId tid, Meta::TypePtr type) -> bool {
        std::cout << std::format("type {} has id {}\n", type->Name (), tid);
        return true;
    });
    return 0;
}
