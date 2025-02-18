#include "constant.hpp"
#include <cassert>
#include <utility>

Constant::Constant (const Cursor &cursor, const Namespace &ns, TypeInfo *parent): TypeInfo (cursor, ns, parent)
{
    assert (cursor.Kind () == CXCursor_EnumConstantDecl);
    name_ = cursor.DisplayName ();
    value_ = clang_getEnumConstantDeclValue (cursor);
}
Constant::~Constant () = default;
std::string Constant::Name () const
{
    return name_;
}
uint64_t    Constant::Value () const
{
    return value_;
}
bool        Constant::ShouldCompile () const
{
    return parent_->ShouldCompile();
}
