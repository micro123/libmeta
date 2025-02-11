#ifndef LIB_HPP
#define LIB_HPP

#include "export.h"
#include "meta-parser.h"

#include <string>

LIB_API
void sayHello();

CLASS(LIB_API MyClass, Enabled, All)
{
    REFL_BODY(MyClass)
public:
    MyClass();
    MyClass(std::string name);
    ~MyClass();
    
    META(Enabled)
    void SayHello();

    int Test(double f) const;

    static void xxx();

private:
    int a, b, c;
    std::string name_;
};

namespace Foo::Bar {
    CLASS(Baz, Enabled) {
        REFL_BODY(Baz)
    public:
        void foo(MyClass *obj) const;

    private:
        int z = 100;
    };

    STRUCT(Bal, Enabled, Fields) {
        REFL_BODY(Bal)
    public:
        float x,y,z;
    };
}

enum CLASS(Van, Enabled, All)
{
    Deep,
    Dark,
    Fantasy,
    Other = 10086,
};

enum
{
    Ni,
    Hao,
};

#endif /* LIB_HPP */
