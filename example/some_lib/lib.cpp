#include "lib.hpp"
#include <cstdio>

void sayHello () {
    puts("你好");
}

void MyClass::Inc (int &origin, f32 *f) {
    origin += 5;
    if (f)
        *f *= *f;
    printf("f = %p\n", f);
}

void MyClass::xxx () {}

MyClass::MyClass()
{

}

MyClass::MyClass(std::string name): name_(std::move(name))
{

}

MyClass::~MyClass() = default;

void MyClass::SayHello ()
{
    printf ("hello from %s\n", name_.c_str ());
}

int  MyClass::Test (double f) const
{
    return f / 2;
}
void Foo::Bar::Baz::foo (MyClass *obj) const
{

}
