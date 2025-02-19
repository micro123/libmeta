#ifndef CLANG_UTILS_HPP
#define CLANG_UTILS_HPP

#include <clang-c/Index.h>
#include <string>
#include <iostream>
#include <utility>

std::string ToString(CXString s);

void Print(CXSourceLocation loc, bool newline = true);

void Print(CXCursor cursor, bool newline = true);

std::pair<unsigned,unsigned> GetSourceRange(CXSourceRange range);

inline std::ostream &operator<< (std::ostream &o, CXString s)
{
    o << ToString(s);
    return o;
}

#endif /* CLANG_UTILS_HPP */
