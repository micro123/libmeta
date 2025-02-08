#ifndef CURSOR_HPP
#define CURSOR_HPP

#include <clang-c/Index.h>

#include <list>

class Cursor final
{
public:
    using Visitor = CXCursorVisitor;
    using List = std::list<Cursor>;

    Cursor(CXCursor cursor);

    List Children(bool filtered = false) const;
    void visitChildren(Visitor visitor, void* data = nullptr) const;
    operator CXCursor() const { return cursor_; }

    bool IsUserType() const;
    bool IsNamespace() const;

private:
    CXCursor cursor_;
};

#endif /* CURSOR_HPP */
