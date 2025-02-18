#ifndef CURSOR_TYPE_HPP
#define CURSOR_TYPE_HPP

#include <clang-c/Index.h>

class CursorType final {
public:
    CursorType(CXType type);
    ~CursorType();

    [[nodiscard]] bool IsReferenced() const;
    [[nodiscard]] bool IsConstQualified() const;
    [[nodiscard]] CXTypeKind Kind() const;
private:
    CXType type_;
};


#endif /* CURSOR_TYPE_HPP */
