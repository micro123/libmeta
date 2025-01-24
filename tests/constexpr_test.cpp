#include <catch_amalgamated.hpp>
#include <utility/constexpr.hpp>

enum foo
{
};
class bar
{
};

TEST_CASE ("Constexpr GetTypeName")
{
    REQUIRE (Meta::GetTypeName<int> () == "int");
    REQUIRE (Meta::GetTypeName<foo> () == "foo");
    REQUIRE (Meta::GetTypeName<bar> () == "bar");
}
