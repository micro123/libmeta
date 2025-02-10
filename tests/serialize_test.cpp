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
    };

    struct Test {
        Vec3f          ok;
        std::string    example; 
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
    auto s = Meta::JsonSerialize(value);
    std::cout << s << std::endl;
}


