#ifndef CURSOR_HPP
#define CURSOR_HPP

#include <clang-c/Index.h>

#include <list>
#include <string>

class Cursor final
{
public:
    using Visitor = CXCursorVisitor;
    using List = std::list<Cursor>;

    Cursor (const CXCursor &cursor);

    [[nodiscard]] List Children(bool filtered = false) const;
    void visitChildren(Visitor visitor, void* data = nullptr) const;
    operator CXCursor() const { return cursor_; }
    [[nodiscard]] std::string DisplayName() const;

    [[nodiscard]] bool IsUserType() const;
    [[nodiscard]] bool IsDataType() const;
    [[nodiscard]] bool IsEnumType() const;
    [[nodiscard]] bool IsNamespace() const;

private:
    CXCursor cursor_;
};

#endif /* CURSOR_HPP */
