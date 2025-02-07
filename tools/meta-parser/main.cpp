#include <clang-c/Index.h>

#include "clang_utils.hpp"

#include <iostream>

/**
 * 需要传入的参数有：
 * 1. compile_headers.txt path
 * 2. auto_register.cpp path
 * 3. -I... -I
 * 4. -D... -D
 * 5. -H... -H
 */
int main(int argc, char const *argv[])
{
    if (argc != 6)
        return 1;
    
    for (int i=0; i<argc; ++i) {
        printf("%d: %s\n", i, argv[i]);
    }
    
    return 0;
}
