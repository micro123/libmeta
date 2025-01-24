#include <catch_amalgamated.hpp>
#include <utility/variant.hpp>

TEST_CASE("Variant Basic")
{
    using namespace Meta;

    Variant v = 42;
    REQUIRE(v.Type() == TypeOf<decltype(42)>());
    v = 3.14;
    REQUIRE(v.Type() == TypeOf<decltype(3.14)>());
}

