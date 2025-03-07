#include "types.h"

namespace Test {
    int Foo::z = 5;
    const int Foo::w = 8;
    int Foo::c[10] = {9,8,7,6,5,4,1,2,3};
    const int Foo::d[10] = {1,1,2,2,3,3,4,4,5,5};

    Foo::Foo(int xx): x(xx), y(x+1), b{1,2,3,4,5,6,7,8,9,0}, zzz(x) {
        puts("Foo");
    }
    Foo::~Foo() {
        puts("~Foo");
    }

    void Foo::print() const {
        puts("void Foo::print() const");
    }

    void Derived::SetValue(int a, int b, int c, int d)
    {
        x = a;
        y = b;
        z = c;
        w = d;
        j = a + b;
        k = a - b;
    }

    void Base1::foo(int v) {
        printf("Base1::foo(int) x = %d, input = %d\n", x, v);
    }

    int Derived::bar() {
        puts("Derived::bar");
        return x + y;
    }
}
