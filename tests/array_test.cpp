#include <catch_amalgamated.hpp>
#include <refl/any.hpp>
#include <refl/method.hpp>
#include "refl/builders.hpp"

#include "types.h"
using namespace Test;

#include <iostream>

TEST_CASE("Array")
{
    using namespace Meta;

    Any v = NDArray();

    auto data = v["data"];
    auto l1 = data[0];
    auto l2 = l1[0];
    auto l3 = l2[0];

    std::cout << v["data"][0][0][0] << "\n";
}
