#ifndef LIB_HPP
#define LIB_HPP

#include "export.h"
#include "meta-parser.h"

#include <string>

LIB_API
void sayHello();

STRUCT(LIB_API SomeData,Fields)
{
    int a, b, c, d;
};

CLASS(LIB_API MyClass,All)
{
    REFL_BODY(MyClass)

    enum {
        Test1,
        Test2,
        Test3,
        Test4,
    };
public:
    MyClass();
    MyClass(std::string name);
    ~MyClass();
    
    META()
    void SayHello();

    int Test(double f) const;

    static void xxx();

private:
    int a, b, c;
    double dd[5];
    std::string name_;
    SomeData data_;
};

namespace Foo::Bar {
    CLASS(Baz,All) {
        REFL_BODY(Baz)
    public:
        void foo(MyClass *obj) const;

    private:
        int z = 100;
    };

    STRUCT(Bal,All) {
        REFL_BODY(Bal)
    public:
        enum {
            A,B,C,D
        };

        float x,y,z;
    };
}

enum CLASS(Van,All)
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
