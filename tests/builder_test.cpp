#include <catch_amalgamated.hpp>
#include <refl/any.hpp>
#include <refl/builders.hpp>
#include <refl/field.hpp>
#include <refl/registry.hpp>

struct Bar {
    int a;
    float b;
};

struct Baz
{
    double d;
};

struct Foo : Bar, Baz
{
    Foo(int yy): y(yy) {}

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

void func() {}
void func(int) {}
void func(double, int) {}

struct S
{
    void func() {}
    void func(int) {}
    void func(double, int) {}
    void func(double, int) const {}
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

    TypeBuilder::NewTypeBuilder<Bar> ()
    .AddField (MakeField("a", &Bar::a))
    .AddField (MakeField("b", &Bar::b));

    TypeBuilder::NewTypeBuilder<Foo> ()
    .AddBaseType (GetTypeId<Bar>(), BaseCvt(Foo,Bar))
    .AddConstructor (
        MethodBuilder::NewMethodBuilder ("Constructor", &Ctor::Of<Foo, int>)
        .Build ()
    )
    .AddField (MakeField("x", &Foo::x))
    .AddField (MakeField("y", &Foo::y))
    .AddField (MakeField("z", &Foo::z))
    .AddMethod (
        MethodBuilder::NewMethodBuilder ("print", &Foo::print)
        .Build ()
    )
    .AddMethod (
        MethodBuilder::NewMethodBuilder ("test", &Foo::test)
        .AddParam (0, "x", 42)
        .Build()
    );
    REQUIRE (TypeOf<Foo>()->Name () == "Foo");

    Any f = Foo(45);
    REQUIRE(f.ValuePtr<Bar>() != nullptr);

    auto obj = Ctor::Of<Foo, int>(6);
    Ref<Baz> bar = obj;
    REQUIRE (obj != nullptr);
    obj.Release ();
    REQUIRE (bar != nullptr);

    auto pf = Overloaded<void()>::Of (func);
    auto pf2 = Overloaded<void(int)>::Of (&S::func);
    auto pf3 = Overloaded<void(double,int)const>::Of (&S::func);
}
