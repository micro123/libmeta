#include <catch_amalgamated.hpp>
#include <refl/field.hpp>

struct Simple {
    int              x;
    const int        y;
    static int       z;
    static const int w;
};

int       Simple::z = 0;
const int Simple::w = 0;

TEST_CASE ("MakeField")
{
    auto x = Meta::MakeField ("x", &Simple::x);
    REQUIRE (x != nullptr);
    REQUIRE (x->IsMember ());
    REQUIRE (!x->IsConst ());
    auto y = Meta::MakeField ("y", &Simple::y);
    REQUIRE (y != nullptr);
    REQUIRE (y->IsMember ());
    REQUIRE (y->IsConst ());
    auto z = Meta::MakeField ("z", &Simple::z);
    REQUIRE (z != nullptr);
    REQUIRE (!z->IsMember ());
    REQUIRE (!z->IsConst ());
    auto w = Meta::MakeField ("w", &Simple::w);
    REQUIRE (w != nullptr);
    REQUIRE (!w->IsMember ());
    REQUIRE (w->IsConst ());
}
