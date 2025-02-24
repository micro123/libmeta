#include <catch_amalgamated.hpp>
#include <refl/field.hpp>

struct Simple {
    int              x;
    const int        y;
    static int       z;
    static const int w;
    int a[10];
    const int b[10];
    static int c[10];
    static const int d[10];
};

int       Simple::z = 21;
const int Simple::w = 42;
int       Simple::c[10] = {1,2,3,4,5,6,7,8,9,0};
const int Simple::d[10] = {1,2,3,4,5,6,7,8,9,0};

TEST_CASE ("MakeField")
{
#define FIELD_TEST(name, m, c, a) \
    do {\
        auto name = Meta::MakeField(#name, &Simple::name); \
        REQUIRE(name != nullptr); \
        REQUIRE(name->IsMember() == m); \
        REQUIRE(name->IsConst() == c); \
        REQUIRE(name->IsArray() == a); \
    } while (0)

    FIELD_TEST(x, true, false, false);
    FIELD_TEST(y, true, true, false);
    FIELD_TEST(z, false, false, false);
    FIELD_TEST(w, false, true, false);
    FIELD_TEST(a, true, false, true);
    FIELD_TEST(b, true, true, true);
    FIELD_TEST(c, false, false, true);
    FIELD_TEST(d, false, true, true);
}

TEST_CASE ("ReadField")
{
    auto px = &Simple::x;
    auto x = Meta::MakeField ("x", &Simple::x);
    auto y = Meta::MakeField ("y", &Simple::y);
    auto z = Meta::MakeField ("z", &Simple::z);
    auto w = Meta::MakeField ("w", &Simple::w);

    int ii = 5;
    char buffer[sizeof(&ii)];
    auto pii = new(buffer) int* {&ii};

    Meta::Any anyi = Meta::Any::RefWrap(&ii);
    auto pi = anyi.ValuePtr<int>();
    REQUIRE((anyi == ii));

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

TEST_CASE ("Bit Fields")
{
    struct Example {
        int a:15;
        int b:13;
        int c:4;
    };
    static_assert(sizeof(Example) == 4);

    auto fa = Meta::BitField<Example, decltype(Example::a)>("a",  0, 15);
    auto fb = Meta::BitField<Example, decltype(Example::b)>("a", 15, 13);
    auto fc = Meta::BitField<Example, decltype(Example::c)>("a", 28,  4);

    Meta::Any v = Example{1,-2,-3};
    REQUIRE(fa->Get(&v) == 1);
    REQUIRE(fb->Get(&v) == -2);
    REQUIRE(fc->Get(&v) == -3);
    
    fa->Set(&v, 100);
    fb->Set(&v, -9);
    fc->Set(&v, -1);
    REQUIRE(fa->Get(&v) == 100);
    REQUIRE(fb->Get(&v) == -9);
    REQUIRE(fc->Get(&v) == -1);
}
