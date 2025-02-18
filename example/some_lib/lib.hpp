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

    int Test(double f) const;

    void Inc(int &origin, f32 *f);

    static void xxx();

private:
    struct META()
    {
        float x,y,z;
    } vec3f;
    int a, b, c;
    double dd[5];
    std::string name_;
    SomeData data_;
};

namespace Foo::Bar {
    CLASS(Baz) {
        REFL_BODY(Baz)
    public:
        void foo(MyClass *obj) const;

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
    };
}

enum CLASS(Van)
{
    Deep,
    Dark,
    META(Disabled) Fantasy,
    Other = 10086,
};

enum
{
    Ni,
    Hao,
};

#endif /* LIB_HPP */
