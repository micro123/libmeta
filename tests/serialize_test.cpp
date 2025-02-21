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
    Meta::Any value = Meta::Any::NewRef<Component>(1.f,2.f,3.f, "fucking deep");
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
    e.AssignValue("NotFound");
    std::cout << e.Value<int> () << std::endl;
    std::cout << e << std::endl;
    e.AssignValue(u8{6});
    std::cout << e.Value<int> () << std::endl;
    std::cout << e << std::endl;
}


TEST_CASE ("Base Type Serialization")
{
    using namespace Meta;
    
    Derived d;
    d.SetValue(1,2,3,4);
    Derived other;
    Any a = &d;
    Any b = &other;
    auto data = XmlSerialize(a, true);
    std::cout << data << '\n';
    XmlDeserialize(b, data);
    REQUIRE(memcmp(&d, &other, sizeof(d)) == 0);
    d.SetValue(4,3,2,1);
    data = JsonSerialize(a, true);
    std::cout << data << '\n';
    JsonDeserialize(b, data);
    REQUIRE(memcmp(&d, &other, sizeof(d)) == 0);
    a["foo"](100);
    b["bar"]();
}
