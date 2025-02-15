#ifndef TYPES_H
#define TYPES_H

#include "meta-parser.h"

#include <string>
#include <cstring>

namespace Test {

STRUCT(Foo,All) {
    int x;
    const int y;
    static int z;
    static const int w;
    int a[10];
    const int b[10];
    static int c[10];
    static const int d[10];

    Foo(int x);
    ~Foo();

    void print() const;
};

STRUCT(ABC,All) {
    int x = 1;
    int y[5] = {1, 2, 3, 4, 5};
};

UNION(Vec3f,All)
{
    struct {
        float x, y, z;
    };
    float data[3];
    bool operator==(const Vec3f& o) const {
        return memcmp(this, &o, sizeof(Vec3f)) == 0;
    }
};

STRUCT(Component,All) {
    Vec3f               ok;
    std::string const   example;

    bool operator==(const Component& o) const = default;
};

}


#endif /* TYPES_H */
