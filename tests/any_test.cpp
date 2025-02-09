#include <catch_amalgamated.hpp>
#include <refl/any.hpp>

TEST_CASE ("Any Basic")
{
    using namespace Meta;

    Any v = 42;
    REQUIRE (v.Type () == TypeOf<decltype (42)> ());
    v = 3.14;
    REQUIRE (v.Type () == TypeOf<decltype (3.14)> ());
    REQUIRE (3.14 == (decltype (3.14)) v);
}

TEST_CASE ("Any Ref")
{
    struct Foo {
        Foo (int x) : x (x)
        {
            puts ("Foo");
        }
        ~Foo ()
        {
            puts ("~Foo");
        }
        void print() const
        {
            printf("x = %d\n", x);
        }
        int x;
    };
    Meta::Any a = Meta::MakeRef<Foo> (55);
    {
        auto b = a;
        b.ValueRef<Foo>().print();
        b = Meta::MakeRef<Foo>(66);
        b.ValueRef<Foo>().print();
        puts("before");
        b = {};
        puts("after");
    }

    Meta::Any b = Meta::Any::New<Foo> (55);
    {
        auto c = b;
        c.ValueRef<Foo>().print();
        c = Meta::MakeRef<Foo>(66);
        c.ValueRef<Foo>().print();
        puts("before");
        c = {};
        puts("after");
    }
}