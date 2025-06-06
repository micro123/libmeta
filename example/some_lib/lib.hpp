#ifndef LIB_HPP
#define LIB_HPP

#include "export.h"
#include "meta-parser.h"

#include <string>

LIB_API
void sayHello();

STRUCT(LIB_API SomeData)
{
    int a, b, c, d;
};

namespace A::B::C::D
{
    STRUCT(SomeParam) {};
}

CLASS(LIB_API MyClass)
{
    REFL_BODY(MyClass)

    enum META() {
        Test1,
        Test2,
        Test3,
        Test4,
    } hello_world_;
public:
    MyClass();
    MyClass(std::string name);
    ~MyClass();
    
    META()
    void SayHello();

    int Test(double f = (1,2,3.14)) const;

    void Inc(int &origin, f32 *f = nullptr);

    static void xxx();

    static void yyy() {}

private:
    struct
    {
        float x,y,z;
    } vec3f;
    int a, b, c;
    double dd[5];
    std::string name_;
    SomeData data_;
    s8 j:3;
    u8 k:5;
};

namespace Foo::Bar {
    CLASS(Baz) {
        REFL_BODY(Baz)
    public:
        void foo(MyClass *obj) const;
        void bar(A::B::C::D::SomeParam const* param) const {}
        void bar(int,double){}
    private:
        int z = 100;
    };

    STRUCT(Bal) {
        REFL_BODY(Bal)
    public:
        enum {
            A,B,C,D
        };

        float x,y,z;
        int a:15;
        int b:13;
        int c:4;
    };
}

enum CLASS(Van)
{
    Deep,
    Dark,
    META(Disabled) Fantasy,
    Other = 10086,
};

enum STRUCT(Simple)
{
    Ni,
    Hao,
};

#endif /* LIB_HPP */
