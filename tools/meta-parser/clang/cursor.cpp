#include "cursor.hpp"
#include <format>
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
std::string Cursor::Spelling () const
{
    return ToString (clang_getCursorSpelling (cursor_));
}

std::string Cursor::SourceFile () const
{
    auto range = clang_Cursor_getSpellingNameRange (cursor_, 0, 0);

    auto start = clang_getRangeStart (range);

    CXFile   file;
    unsigned line, column, offset;

    clang_getFileLocation (start, &file, &line, &column, &offset);

    std::string filename = std::format ("{}:{}:{}", ToString (clang_getFileName (file)), line, column);

    return filename;
}

std::string Cursor::LangType () const
{
    auto type = clang_getCursorType (cursor_);
    return ToString (clang_getTypeSpelling (type));
}

bool Cursor::IsPublicAccess () const
{
    return clang_getCXXAccessSpecifier(cursor_) == CX_CXXPublic;
}

bool Cursor::IsUserType () const
{
    return IsEnumType () || IsDataType ();
}

bool Cursor::IsNamespace () const
{
    return cursor_.kind == CXCursor_Namespace;
}
bool Cursor::IsAnonymous () const
{
    return clang_Cursor_isAnonymous (cursor_);
}

bool Cursor::IsBitField () const
{
    return clang_Cursor_isBitField(cursor_);
}

CursorType Cursor::Type () const
{
    return CursorType (clang_getCursorType(cursor_));
}

bool Cursor::IsDataType () const
{
    return cursor_.kind == CXCursor_StructDecl || cursor_.kind == CXCursor_ClassDecl || cursor_.kind == CXCursor_UnionDecl;
}
bool Cursor::IsEnumType () const
{
    return cursor_.kind == CXCursor_EnumDecl;
}
