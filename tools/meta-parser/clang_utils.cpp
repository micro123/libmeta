#include "clang_utils.hpp"

std::string ToString (CXString s)
{
    std::string r;
    auto cstring = clang_getCString(s);
    if (cstring) {
        r = cstring;
    }
    clang_disposeString(s);
    return r;
}
