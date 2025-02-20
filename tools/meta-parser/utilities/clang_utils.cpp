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

void Print (CXCursor cursor, bool newline)
{
    std::cout << std::format ("Cursor {{{}: {}}} Hash: 0x{:08X}", ToString (clang_getCursorKindSpelling (cursor.kind)),
                              ToString (clang_getCursorDisplayName (cursor)), clang_hashCursor (cursor));
    if (newline)
        std::cout << std::endl;
}

void Print (CXType type, bool newline)
{
    std::cout << std::format ("Type {{{}: {}}}", ToString (clang_getTypeKindSpelling (type.kind)),
                              ToString (clang_getTypeSpelling (type)));
    if (newline)
        std::cout << std::endl;
}

std::pair<unsigned, unsigned> GetSourceRange (CXSourceRange range)
{
    if (clang_Range_isNull(range))
        return {0,0};
    auto beg = clang_getRangeStart(range);
    auto end = clang_getRangeEnd(range);

    unsigned offset0, offset1;
    clang_getFileLocation(beg, nullptr, nullptr, nullptr, &offset0);
    clang_getFileLocation(end, nullptr, nullptr, nullptr, &offset1);
    if (offset1 > offset0) {
        return {offset0, offset1 - offset0};
    }

    return {0,0};
}
