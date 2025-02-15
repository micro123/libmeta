#include <catch_amalgamated.hpp>
#include <refl/any.hpp>
#include <refl/method.hpp>
#include "refl/builders.hpp"

TEST_CASE ("Any Basic")
{
    using namespace Meta;

    Any v = 42;
    REQUIRE (v.Type () == TypeOf<decltype (42)> ());
    v = 3.14;
    REQUIRE (v.Type () == TypeOf<decltype (3.14)> ());
    REQUIRE (3.14 == v.Value<double> ());
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

int print(int x) { puts("printed!"); return 42 + x; }

TEST_CASE("Any Callable") {
    Meta::Any p = Meta::MakeMethod("print", &print);
    auto r = p(42);
    REQUIRE(r == 42 + 42);
}

TEST_CASE ("Any Child & Subscript")
{
    struct ABC
    {
        int x = 1;
        int y[5] = {1, 2, 3, 4, 5};
    };

    using namespace Meta;
    (void)TypeBuilder::NewTypeBuilder<ABC> ()
    .AddField (MakeField ("x", &ABC::x))
    .AddField (MakeField ("y", &ABC::y))
    .Register ();

    Any a = ABC();
    REQUIRE((a["x"] == 1));
    REQUIRE((a["y"] == 1));
    Any y = a["y"];
    Any z = y[1];
    REQUIRE(z == 2);
}
