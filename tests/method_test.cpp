#include <catch_amalgamated.hpp>
#include <refl/method.hpp>

double foo (int x, int y, int z)
{
    printf ("in %s, x = %d, y = %d, z = %d\n", __FUNCTION__, x, y, z);
    return 3.14;
}
struct A {
    void a ()
    {
        puts (__FUNCTION__);
    }
    void b () const
    {
        puts (__FUNCTION__);
    }
    void c () volatile
    {
        puts (__FUNCTION__);
    }
    static void d ()
    {
        puts (__FUNCTION__);
    }
};

TEST_CASE ("Method Simple")
{
    auto m = Meta::MakeMethod ("foo", &foo);
    REQUIRE (m->Name () == "foo");
    auto a = Meta::MakeMethod ("a", &A::a);
    REQUIRE (a->IsMember ());
    REQUIRE (!a->IsConst ());
    REQUIRE (!a->IsVolatile ());
    auto b = Meta::MakeMethod ("b", &A::b);
    REQUIRE (b->IsMember ());
    REQUIRE (b->IsConst ());
    REQUIRE (!b->IsVolatile ());
    auto c = Meta::MakeMethod ("c", &A::c);
    REQUIRE (c->IsMember ());
    REQUIRE (!c->IsConst ());
    REQUIRE (c->IsVolatile ());
    auto d = Meta::MakeMethod ("d", &A::d);
    REQUIRE (!d->IsMember ());
    REQUIRE (!d->IsConst ());
    REQUIRE (!d->IsVolatile ());
}

TEST_CASE ("Method Call")
{
    A obj;

    auto f = Meta::MakeMethod ("foo", &foo);
    auto r = f->Invoke (1, 2, 3);
    REQUIRE(r.Type() == Meta::TypeOf<double>());
    REQUIRE((double)r == foo(1,2,3));

    auto a = Meta::MakeMethod ("a", &A::a);
    a->Invoke (obj);

    auto b = Meta::MakeMethod ("b", &A::b);
    b->Invoke (obj);

    auto c = Meta::MakeMethod ("c", &A::c);
    c->Invoke (obj);

    auto d = Meta::MakeMethod ("d", &A::d);
    d->Invoke ();
}