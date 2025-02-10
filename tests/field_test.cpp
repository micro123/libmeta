#include <catch_amalgamated.hpp>
#include <refl/field.hpp>

struct Simple {
    int              x;
    const int        y;
    static int       z;
    static const int w;
};

int       Simple::z = 21;
const int Simple::w = 42;

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

TEST_CASE ("ReadField")
{
    auto px = &Simple::x;
    auto x = Meta::MakeField ("x", &Simple::x);
    auto y = Meta::MakeField ("y", &Simple::y);
    auto z = Meta::MakeField ("z", &Simple::z);
    auto w = Meta::MakeField ("w", &Simple::w);

    Meta::Any i;
    Meta::Any obj = Simple{1,2};
    i = x->Get(&obj);
    REQUIRE(i.ValueRef<decltype(Simple::x)>() == 1);
    i = y->Get(&obj);
    REQUIRE(i.ValueRef<decltype(Simple::y)>() == 2);
    i = z->Get(&obj);
    REQUIRE(i.ValueRef<decltype(Simple::z)>() == Simple::z);
    i = z->Get({});
    REQUIRE(i.ValueRef<decltype(Simple::z)>() == Simple::z);
    i = w->Get(&obj);
    REQUIRE(i.ValueRef<decltype(Simple::w)>() == Simple::w);
    i = w->Get({});
    REQUIRE(i.ValueRef<decltype(Simple::w)>() == Simple::w);
}

TEST_CASE ("WriteField")
{
    auto x = Meta::MakeField ("x", &Simple::x);
    auto y = Meta::MakeField ("y", &Simple::y);
    auto z = Meta::MakeField ("z", &Simple::z);
    auto w = Meta::MakeField ("w", &Simple::w);

    Meta::Any obj = Simple{.x = 1, .y = 2}, i;
    Simple *simple = obj.ValuePtr<Simple>();
    i = x->Set(&obj, 10);
    REQUIRE(simple->x == i.ValueRef<decltype(Simple::x)>());
    REQUIRE(simple->x == 10);
    i = y->Set(&obj, 20);
    REQUIRE(simple->y == i.ValueRef<decltype(Simple::y)>());
    i = z->Set(&obj, 30);
    REQUIRE(simple->z == i.ValueRef<decltype(Simple::z)>());
    i = w->Set(&obj, 40);
    REQUIRE(simple->w == i.ValueRef<decltype(Simple::w)>());
}
