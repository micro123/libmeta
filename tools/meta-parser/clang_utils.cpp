#include "clang_utils.hpp"

#include <format>
#include <iostream>

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

void Print(CXSourceLocation loc, bool newline)
{
    CXFile file;
    unsigned int line, column, offset;
    clang_getFileLocation(loc, &file, &line, &column, &offset);
    std::cout << "SourceLoc " << clang_getFileName(file) << std::format(":{}:{}:{}", line, column, offset);
    if (newline)
        std::cout << std::endl;
}

void Print(CXCursor cursor, bool newline)
{
    // cursor {type: spelling}
    std::cout << std::format("Cursor {{{}: {}}}",
        ToString(clang_getCursorKindSpelling(cursor.kind)),
        ToString(clang_getCursorDisplayName(cursor)));
    if (newline)
        std::cout << std::endl;
}
