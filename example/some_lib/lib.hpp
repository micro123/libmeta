#ifndef LIB_HPP
#define LIB_HPP

#include "export.h"
#include "meta-parse.h"

#include <string>

LIB_API
void sayHello();

CLASS(MyClass, mode=all, desc=some class)
{
    REFL_BODY(MyClass)
public:
    MyClass();
    MyClass(std::string name);
    ~MyClass();
    
    META(keep, desc=I will say hello to you!)
    void SayHello();

private:
    std::string name_;
};

namespace Foo::Bar {
    CLASS(Baz, mode=public) {
        REFL_BODY(Baz)
    public:
        void foo() const;

    private:
        int z = 100;
    };

    STRUCT(Bal, mode=fields) {
        REFL_BODY(Bal)
    public:
        float x,y,z;
    };
}

#endif /* LIB_HPP */
