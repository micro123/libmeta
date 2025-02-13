#include <catch_amalgamated.hpp>
#include <iostream>
#include <refl/any.hpp>
#include <refl/builders.hpp>
#include <serialization/jsonify.hpp>
#include <serialization/xml.hpp>

TEST_CASE ("Serialization")
{
    union Vec3f
    {
        struct {
            float x, y, z;
        };
        float data[3];
        bool operator==(const Vec3f& o) const = default;
    };

    struct Test {
        Vec3f               ok;
        std::string const   example;

        bool operator==(const Test& o) const = default;
    };

    (void)Meta::TypeBuilder::NewTypeBuilder<Vec3f>("Vec3f")
    .AddField (Meta::MakeField ("x", &Vec3f::x))
    .AddField (Meta::MakeField ("y", &Vec3f::y))
    .AddField (Meta::MakeField ("z", &Vec3f::z))
    .AddField (Meta::MakeField ("data", &Vec3f::data))
    .Register ();

    (void)Meta::TypeBuilder::NewTypeBuilder<Test>("Test")
    .AddField (Meta::MakeField ("vec3", &Test::ok))
    .AddField (Meta::MakeField ("example", &Test::example))
    .Register ();

    Meta::Any value = Meta::Any::New<Test>(1.f,2.f,3.f, "fucking deep");
    auto s = Meta::JsonSerialize(value);
    std::cout << s << std::endl;
    Meta::Any newValue = Test{};
    Meta::JsonDeserialize(newValue, s);
    REQUIRE((value == newValue));
    std::cout << Meta::JsonSerialize(newValue, true) << std::endl;

    auto x = Meta::XmlSerialize(value);
    std::cout << x << std::endl;
    newValue = Test{};
    Meta::XmlDeserialize(newValue, x);
    REQUIRE((value == newValue));
    std::cout << Meta::XmlSerialize(newValue, true) << std::endl;
}


