#include <clang-c/Index.h>

#include "clang_utils.hpp"

#include <iostream>

#define MAJOR 1
#define MINOR 2
#define PATCH 3
#define TYPE  beta
#define COUNT 1023
#define BIULD_DATE 250207
#define GIT_SHA1   abcdefgh

#define STRINGIFY(x) STRINGIFY_(x)
#define STRINGIFY_(x) #x

#define BUILD_INFO_PLACEHOLDER_
#define BUILD_INFO_PLACEHOLDER_rc    "rc.",
#define BUILD_INFO_PLACEHOLDER_alpha "alpha.",
#define BUILD_INFO_PLACEHOLDER_beta  "beta.",
#define TAKE_FIRST_ARG(val,...) val
#define TAKE_BUILD_INFO__(arg1_or_junk) TAKE_FIRST_ARG(arg1_or_junk "")
#define TAKE_BUILD_INFO_(val) TAKE_BUILD_INFO__(BUILD_INFO_PLACEHOLDER_##val)
#define TAKE_BUILD_INFO(t) TAKE_BUILD_INFO_(t)


#define VERSION_STR(x,y,z,t,c) \
STRINGIFY(x) "." STRINGIFY(y) "." STRINGIFY(z) "-" \
TAKE_BUILD_INFO(t) STRINGIFY(c)

#define VERSION_STR_MAGIC(x,y,z,t,c,d,g) \
VERSION_STR(x,y,z,t,c) "+" STRINGIFY(d) "." STRINGIFY(g)

const char *const kVerStr = VERSION_STR(MAJOR,MINOR,PATCH,TYPE,COUNT);
const char *const kVerStrMagic = VERSION_STR_MAGIC(MAJOR,MINOR,PATCH,TYPE,COUNT,BIULD_DATE,GIT_SHA1);

/**
 * 需要传入的参数有：
 * 1. 
 */
int main(int argc, char const *argv[])
{
    std::cout << kVerStr << std::endl;
    std::cout << kVerStrMagic << std::endl;
    std::cout << ToString(clang_getClangVersion()) << std::endl;
    /* code */
    return 0;
}
