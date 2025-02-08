#include "cursor.hpp"
#include "utilities/clang_utils.hpp"

Cursor::Cursor (const CXCursor &cursor) : cursor_ (cursor) {}

Cursor::List Cursor::Children (bool filtered) const
{
    static Visitor s_visitor = +[] (CXCursor c, CXCursor p, CXClientData d) {
        List *children = static_cast<List *> (d);
        children->emplace_back (c);
        if (c.kind == CXCursor_LinkageSpec || c.kind == CXCursor_UnexposedDecl)
            return CXChildVisit_Recurse;
        return CXChildVisit_Continue;
    };
    static Visitor s_visitor_filtered = +[] (CXCursor c, CXCursor p, CXClientData d) {
        List *children = static_cast<List *> (d);
        auto  loc      = clang_getCursorLocation (c);
        if (clang_Location_isFromMainFile (loc) || !clang_Location_isInSystemHeader (loc))
        {
            children->emplace_back (c);
        }
        if (c.kind == CXCursor_LinkageSpec || c.kind == CXCursor_UnexposedDecl)
            return CXChildVisit_Recurse;
        return CXChildVisit_Continue;
    };

    List result;
    visitChildren (filtered ? s_visitor_filtered : s_visitor, &result);
    return result;
}

void Cursor::visitChildren (Visitor visitor, void *data) const
{
    clang_visitChildren (cursor_, visitor, data);
}

std::string Cursor::DisplayName () const
{
    return ToString (clang_getCursorDisplayName (cursor_));
}

bool Cursor::IsUserType () const
{
    return IsEnumType () || IsDataType ();
}

bool Cursor::IsNamespace () const
{
    return cursor_.kind == CXCursor_Namespace;
}

bool Cursor::IsDataType () const
{
    return cursor_.kind == CXCursor_StructDecl || cursor_.kind == CXCursor_ClassDecl || cursor_.kind == CXCursor_UnionDecl;
}
bool Cursor::IsEnumType () const
{
    return cursor_.kind == CXCursor_EnumDecl;
}
