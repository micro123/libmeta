#include "lib.hpp"
#include <cstdio>

void sayHello () {
    puts("你好");
}

MyClass::MyClass()
{

}

MyClass::MyClass(std::string name): name_(std::move(name))
{

}

MyClass::~MyClass() = default;

void MyClass::SayHello() {
    printf("hello from %s\n", name_.c_str());
}
