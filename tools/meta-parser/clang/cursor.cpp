#include "cursor.hpp"

Cursor::Cursor (CXCursor cursor): cursor_(cursor) {}

Cursor::List Cursor::Children (bool filter_source) const
{
    static Visitor s_visitor = +[](CXCursor c, CXCursor p, CXClientData d) {
        List *children = (List*)d;
        children->emplace_back(c);
        //if (c.kind == CXCursor_LastPreprocessing)
        //    return CXChildVisit_Break;
        if (c.kind == CXCursor_LinkageSpec
            || c.kind == CXCursor_UnexposedDecl
            || c.kind == CXCursor_Namespace)
            return CXChildVisit_Recurse;
        return CXChildVisit_Continue;
    };
    static Visitor s_visitor_filtered = +[](CXCursor c, CXCursor p, CXClientData d) {
        List *children = (List*)d;
        auto loc = clang_getCursorLocation(c);
        if (clang_Location_isFromMainFile(loc) || !clang_Location_isInSystemHeader(loc))
        {
            children->emplace_back(c);
        }
        // if (c.kind == CXCursor_LastPreprocessing)
        //     return CXChildVisit_Break;
        if (c.kind == CXCursor_LinkageSpec
            || c.kind == CXCursor_UnexposedDecl
            || c.kind == CXCursor_Namespace)
            return CXChildVisit_Recurse;
        return CXChildVisit_Continue;
    };

    List result;
    visitChildren(filter_source ? s_visitor_filtered : s_visitor, &result);
    return result;
}

void Cursor::visitChildren(Visitor visitor, void* data) const
{
    clang_visitChildren(cursor_, visitor, data);
}

bool Cursor::IsUserType () const
{
    return 
        cursor_.kind == CXCursor_StructDecl ||
        cursor_.kind == CXCursor_ClassDecl ||
        cursor_.kind == CXCursor_EnumDecl ||
        cursor_.kind == CXCursor_UnionDecl;
}

bool Cursor::IsNamespace () const
{
    return cursor_.kind == CXCursor_Namespace;
}
