#include <catch_amalgamated.hpp>
#include <refl/any.hpp>
#include <refl/builders.hpp>
#include <refl/field.hpp>
#include <refl/registry.hpp>

struct Foo
{
    int x;
    const int y;
    static int z;

    void print() const
    {
        printf("Foo: x=%d, y=%d, z=%d\n", x, y, z);
    }

    void test(int x = 42) const
    {

    }
};

int Foo::z = 42;

enum MyEnum
{
    A = 0,
    B,
    C,
};

TEST_CASE ("TYPE BUILDER")
{
    using namespace Meta;

    auto type = TypeBuilder::NewTypeBuilder<Foo> ()
    .AddField (MakeField("x", &Foo::x))
    .AddField (MakeField("y", &Foo::y))
    .AddField (MakeField("z", &Foo::z))
    .AddMethod (
        MethodBuilder::NewMethodBuilder ("print", &Foo::print)
        .Build ()
    )
    .AddMethod (
        MethodBuilder::NewMethodBuilder ("test", &Foo::test)
        .AddParam ("x", NParamId (Foo::test, 0), 42)
        .Build()
    )
    .Register ();
    REQUIRE (type->Name () == "Foo");
}
