#include <catch_amalgamated.hpp>
#include <iostream>
#include <refl/any.hpp>
#include <refl/builders.hpp>
#include <serialization/jsonify.hpp>
#include <serialization/xml.hpp>

#include "types.h"
using namespace Test;

TEST_CASE ("Serialization")
{
    Meta::Any value = Meta::Any::New<Component>(1.f,2.f,3.f, "fucking deep");
    auto s = Meta::JsonSerialize(value);
    std::cout << s << std::endl;
    Meta::Any newValue = Component{};
    Meta::JsonDeserialize(newValue, s);
    REQUIRE((value == newValue));
    std::cout << Meta::JsonSerialize(newValue, true) << std::endl;

    auto x = Meta::XmlSerialize(value);
    std::cout << x << std::endl;
    newValue = Component{};
    Meta::XmlDeserialize(newValue, x);
    REQUIRE((value == newValue));
    std::cout << Meta::XmlSerialize(newValue, true) << std::endl;
}

TEST_CASE ("Enum Serialize")
{
    using namespace Meta;
    Any e = SomeEnum::First;
    std::cout << e.Value<int> () << std::endl;
    std::cout << e << std::endl;
    e = SomeEnum::Second;
    std::cout << e.Value<int> () << std::endl;
    std::cout << e << std::endl;
    e = SomeEnum::Last;
    std::cout << e.Value<int> () << std::endl;
    std::cout << e << std::endl;
    e.Type ()->ConvertFrom ("Second", e, GetTypeId<cstr>());
    std::cout << e.Value<int> () << std::endl;
    std::cout << e << std::endl;
    e.Type ()->ConvertFrom<s8> (s8{-1}, e);
    std::cout << e.Value<int> () << std::endl;
    std::cout << e << std::endl;
}

