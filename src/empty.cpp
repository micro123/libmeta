#include "exports.h"

LIBMETA_API
int foo() {
    u8 a = 0x12;
    return a ^ 55;
}
