#include <catch_amalgamated.hpp>
#include "refl/type.hpp"
#include "utility/constexpr.hpp"

TEST_CASE ("TypeId BasicType")
{
    auto x = Meta::GetTypeId<s8> ();
    REQUIRE ((x != Meta::NULL_TYPE_ID));
    auto t = Meta::TypeOf<u8> ();
    REQUIRE ((t != nullptr));
    REQUIRE (t->Name () == "u8");
}

TEST_CASE ("TypeFalgs")
{
    constexpr u32 uint_flags    = Meta::CalcTypeFlags<u32> ();
    constexpr u32 uintref_flags = Meta::CalcTypeFlags<u32 &> ();

    struct Vec2 {
        f32 x, y;
    };
    constexpr u32 vec2_flags   = Meta::CalcTypeFlags<const Vec2 &> ();
    constexpr u32 string_flags = Meta::CalcTypeFlags<Meta::sview *const> ();
}

TEST_CASE ("Typeid Ignore CVRef")
{
    struct Foo {};
    REQUIRE(Meta::GetTypeId<Foo>() == Meta::GetTypeId<Foo&>());
    REQUIRE(Meta::GetTypeId<Foo>() == Meta::GetTypeId<Foo*>());
    REQUIRE(Meta::GetTypeId<Foo>() == Meta::GetTypeId<const Foo>());
    REQUIRE(Meta::GetTypeId<Foo>() == Meta::GetTypeId<const Foo&>());
    REQUIRE(Meta::GetTypeId<Foo>() == Meta::GetTypeId<const Foo*>());
    REQUIRE(Meta::GetTypeId<Foo>() == Meta::GetTypeId<volatile Foo>());
    REQUIRE(Meta::GetTypeId<Foo>() == Meta::GetTypeId<volatile Foo&>());
    REQUIRE(Meta::GetTypeId<Foo>() == Meta::GetTypeId<volatile Foo*>());
    REQUIRE(Meta::GetTypeId<int>() == Meta::GetTypeId<int&>());
}
