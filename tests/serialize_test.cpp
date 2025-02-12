#include <catch_amalgamated.hpp>
#include <iostream>
#include <refl/any.hpp>
#include <refl/builders.hpp>
#include <serialization/jsonify.hpp>

TEST_CASE ("Serialization")
{
    struct Vec3f
    {
        float x, y, z;
        bool operator==(const Vec3f& o) const = default;
    };

    struct Test {
        Vec3f          ok;
        std::string    example;

        bool operator==(const Test& o) const = default;
    };

    (void)Meta::TypeBuilder::NewTypeBuilder<Vec3f>("Vec3f")
    .AddField (Meta::MakeField ("x", &Vec3f::x))
    .AddField (Meta::MakeField ("y", &Vec3f::y))
    .AddField (Meta::MakeField ("z", &Vec3f::z))
    .Register ();

    (void)Meta::TypeBuilder::NewTypeBuilder<Test>("Test")
    .AddField (Meta::MakeField ("vec3", &Test::ok))
    .AddField (Meta::MakeField ("example", &Test::example))
    .Register ();

    Meta::Any value = Meta::Any::New<Test>(1.f,2.f,3.f, "fucking");
    auto s = Meta::JsonSerialize(value, true);
    std::cout << s << std::endl;
    Meta::Any newValue = Test{};
    Meta::JsonDeserialize(newValue, s);
    REQUIRE((value == newValue));
}


