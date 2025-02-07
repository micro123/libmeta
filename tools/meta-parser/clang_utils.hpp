#ifndef CLANG_UTILS_HPP
#define CLANG_UTILS_HPP

#include <clang-c/CXString.h>
#include <string>

std::string ToString(CXString s);

#endif /* CLANG_UTILS_HPP */
