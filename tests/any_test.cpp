#include <catch_amalgamated.hpp>
#include <refl/any.hpp>

TEST_CASE("Any Basic")
{
    using namespace Meta;

    Any v = 42;
    REQUIRE(v.Type() == TypeOf<decltype(42)>());
    v = 3.14;
    REQUIRE(v.Type() == TypeOf<decltype(3.14)>());
}

