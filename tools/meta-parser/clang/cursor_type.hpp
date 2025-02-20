#ifndef CURSOR_TYPE_HPP
#define CURSOR_TYPE_HPP

#include <clang-c/Index.h>
#include <string>

class CursorType final {
public:
    CursorType(CXType type);
    ~CursorType();

    [[nodiscard]] bool IsReferenced() const;
    [[nodiscard]] bool IsConstQualified() const;
    [[nodiscard]] CXTypeKind Kind() const;
    [[nodiscard]] unsigned GetNumArguments() const;
    [[nodiscard]] std::string Spelling() const;
    [[nodiscard]] operator CXType () const { return type_; }

private:
    CXType type_;
};


#endif /* CURSOR_TYPE_HPP */
