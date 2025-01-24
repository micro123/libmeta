#include <catch_amalgamated.hpp>
#include <refl/method.hpp>

void foo() {}
struct A {
    void a() {}
    void b() const {}
    void c() volatile {}
    static void d() {}
};

TEST_CASE ("Method Simple")
{
    auto m = Meta::MakeMethod("foo", &foo);
    REQUIRE(m->Name() == "foo");
    auto a = Meta::MakeMethod("a", &A::a);
    REQUIRE( a->IsMember());
    REQUIRE(!a->IsConst());
    REQUIRE(!a->IsVolatile());
    auto b = Meta::MakeMethod("b", &A::b);
    REQUIRE( b->IsMember());
    REQUIRE( b->IsConst());
    REQUIRE(!b->IsVolatile());
    auto c = Meta::MakeMethod("c", &A::c);
    REQUIRE( c->IsMember());
    REQUIRE(!c->IsConst());
    REQUIRE( c->IsVolatile());
    auto d = Meta::MakeMethod("d", &A::d);
    REQUIRE(!d->IsMember());
    REQUIRE(!d->IsConst());
    REQUIRE(!d->IsVolatile());
}
