#include "user_type.hpp"
#include <cassert>
#include <utility>

#include "field.hpp"
#include "function.hpp"
#include "constant.hpp"

LanguageType::LanguageType (const Namespace &ns, std::string name, const Cursor &cursor): TypeInfo (cursor, ns)
{
    if (clang_Cursor_isAnonymous (cursor))
        name_ = "";
    else
        name_ = std::move(name);
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
        if (x.Kind () == CXCursor_FieldDecl)
            fields_.emplace_back (new Field(x, ns, this));
        else if (x.Kind () == CXCursor_CXXMethod)
            functions_.emplace_back (new Function (x, ns, this));
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
            constants_.emplace_back (new Constant(x, ns, this));
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
