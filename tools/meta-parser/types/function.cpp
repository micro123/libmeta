#include "function.hpp"
#include "utilities/clang_utils.hpp"
#include "parser/code_parser.hpp"

static std::string ArgGetDefault(Cursor c) {
    std::string result;
    auto children = c.Children();
    bool found = false;
    for (auto const &x: children)
    {
        auto kind = x.Kind();
        // many kind of statements
        if (kind >= CXCursor_FirstExpr && kind <= CXCursor_LastExpr)
        {
            auto [offset,length] = GetSourceRange(clang_getCursorExtent(x));
            if (offset)
                result = MetaParser::Current()->GetSource(offset, length);
            break;
        }
    }
    return result;
}

Function::Function (const Cursor &cursor, const Namespace &ns, TypeInfo *parent) : TypeInfo (cursor, ns, parent)
{
    name_ = cursor.Spelling ();
    auto count = clang_Cursor_getNumArguments(cursor);
    for (int i = 0; i < count; ++i) {
        Cursor arg = clang_Cursor_getArgument(cursor, i);
        auto name = arg.DisplayName();
        auto def  = ArgGetDefault(arg);
        parameters_.emplace_back(std::move(name), std::move(def));
    }

}
Function::~Function () = default;
bool Function::ShouldCompile () const
{
    if (parent_->ShouldCompile())
    {
        return GetCursor().IsPublicAccess() || meta_info_.IsEnabled();
    }
    return false;
}
