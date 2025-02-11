#include <catch_amalgamated.hpp>
#include <utility/string_name.hpp>

TEST_CASE ("StringName")
{
    using namespace Meta;


    StringName s1 = SNAME("Hello");
    StringName s2{"World"};
    StringName s3{"Hello"};

    REQUIRE((s1 == s3));
    REQUIRE((s1 != s2));
}
