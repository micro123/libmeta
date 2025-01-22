#include <catch_amalgamated.hpp>

#include "refl/registry.hpp"
#include "refl/type.hpp"

TEST_CASE("Registry Basic Types") {
    using namespace Meta;

#define NOT_NULL(x) \
    REQUIRE(inst.Get(GetTypeId<x>()) != nullptr)

    auto &inst = Registry::Instance();
    FUNDAMENTAL_TYPES(NOT_NULL);
}

TEST_CASE("Registry Basic Type Names") {
    using namespace Meta;

#define NAME_EQ(x) \
    REQUIRE(inst.Get(GetTypeId<x>())->Name() == #x)

    auto &inst = Registry::Instance();
    FUNDAMENTAL_TYPES(NAME_EQ);
}
