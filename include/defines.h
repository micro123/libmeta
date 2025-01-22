#ifndef DEFINES_H
#define DEFINES_H

#include <stdint.h>

#define INTS(X) \
    X(8);  \
    X(16); \
    X(32); \
    X(64)

#define U(N) typedef uint##N##_t u##N
#define S(N) typedef  int##N##_t s##N

INTS(U);
INTS(S);

typedef float  f32;
typedef double f64;

#undef INTS
#undef U
#undef S

#define PASTE(X) PASTE_(X)
#define PASTE_(X) X

#endif /* DEFINES_H */
