#include "refl/registry.hpp"
#include <catch_amalgamated.hpp>
#include "refl/type.hpp"
#include <format>
#include <iostream>

TEST_CASE ("Registry Basic Types")
{
    using namespace Meta;

#define NOT_NULL(x) REQUIRE (inst.Get (GetTypeId<x> ()) != nullptr)

    auto &inst = Registry::Instance ();
    FUNDAMENTAL_TYPES (NOT_NULL);

    inst.VisitTypes([](TypeId tid, TypePtr ptr) -> bool {
        std::cout << std::format("name = {}, id = {}\n", ptr->Name(), tid.operator std::string());
        return true;
    });
}

TEST_CASE ("Registry Basic Type Names")
{
    using namespace Meta;

#define NAME_EQ(x) REQUIRE (inst.Get (GetTypeId<x> ())->Name () == #x)

    auto &inst = Registry::Instance ();
    FUNDAMENTAL_TYPES (NAME_EQ);
}

TEST_CASE ("Registry Basic Type Size")
{
    using namespace Meta;

#define SIZE_EQ(x) REQUIRE (inst.Get (GetTypeId<x> ())->Size () == sizeof(x))

    auto &inst = Registry::Instance ();
    FUNDAMENTAL_TYPES (SIZE_EQ);
}
