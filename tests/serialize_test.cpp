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

    (void)Meta::TypeBuilder::NewTypeBuilder<Vec3f>("Vec3f")
    .AddField (Meta::MakeField ("x", &Vec3f::x))
    .AddField (Meta::MakeField ("y", &Vec3f::y))
    .AddField (Meta::MakeField ("z", &Vec3f::z))
    .Register ();

    Meta::Any value = Meta::Any::New<Vec3f>(1.f,2.f,3.f);
    auto s = Meta::JsonSerialize(value);
    std::cout << s << std::endl;
}


