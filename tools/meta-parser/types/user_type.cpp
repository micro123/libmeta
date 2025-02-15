#include "user_type.hpp"
#include <cassert>
#include <utility>

#include "field.hpp"
#include "function.hpp"
#include "constant.hpp"

#include "utilities/clang_utils.hpp"

LanguageType::LanguageType (const Namespace &ns, std::string name, const Cursor &cursor): TypeInfo (cursor, ns)
{
    if (clang_Cursor_isAnonymous (cursor))
        name_ = "";
    else
        name_ = std::move(name);
    if (!ShouldCompile())
        return;
    assert (cursor.IsUserType());
    if (cursor.IsEnumType ())
        ParseEnumType (cursor);
    else
        ParseDataType (cursor);
}
LanguageType::~LanguageType ()
{
    for (auto &x: fields_) delete x;
    for (auto &x: functions_) delete x;
    for (auto &x: constants_) delete x;
}
void LanguageType::ParseDataType (const Cursor &cursor)
{
    // TODO: get fields and functions
    auto ns = namespace_;
    ns.Push (name_);
    auto children = cursor.Children ();
    for (auto &x: children)
    {
        // Print(x);
        auto const kind = x.Kind();
        if (kind == CXCursor_FieldDecl || kind == CXCursor_VarDecl) {
            auto f = new Field(x, ns, this);
            if (f->ShouldCompile())
                fields_.emplace_back (f);
            else
                delete f;
        }
        else if (kind == CXCursor_CXXMethod)
        {
            auto name = x.DisplayName();
            if (name.find("operator") == 0)
                continue;
            auto f = new Function(x, ns, this);
            if (f->ShouldCompile())
                functions_.emplace_back (f);
            else
                delete f;
        }
        else if (kind == CXCursor_EnumDecl)
        {
            ParseEnumType(x);
        }
        else if ((kind == CXCursor_StructDecl || kind == CXCursor_UnionDecl) && clang_Cursor_isAnonymous(x))
        {
            ParseDataType(x);
        }
    }
}
void LanguageType::ParseEnumType (const Cursor &cursor)
{
    // TODO: get enums
    auto ns = namespace_;
    ns.Push (name_);
    auto children = cursor.Children ();
    for (auto &x: children)
    {
        if (x.Kind () == CXCursor_EnumConstantDecl)
        {
            auto c = new Constant(x, ns, this);
            if (c->ShouldCompile())
                constants_.emplace_back (new Constant(x, ns, this));
            else
                delete c;
        }
    }
}

std::string LanguageType::FullName () const
{
    return namespace_.GetFullQualifiedName (name_);
}
std::string LanguageType::Name () const
{
    return name_;
}
bool LanguageType::ShouldCompile () const
{
    return enabled_;
}
const std::list<Field *>    &LanguageType::Fields () const
{
    return fields_;
}
const std::list<Function *> &LanguageType::Functions () const
{
    return functions_;
}
const std::list<Constant *> &LanguageType::Constants () const
{
    return constants_;
}
