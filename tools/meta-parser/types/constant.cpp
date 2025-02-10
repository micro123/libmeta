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
    if (parent_->ShouldCompile())
    {
        auto &pm = parent_->GetMetaInfo();
        auto const disabled_ = meta_info_.GetFlag(NativeProperty::Disabled);
        if ((pm.GetFlag(NativeProperty::All) || pm.GetFlag(NativeProperty::Constants)) && !disabled_)
            return true;
        if (pm.GetFlag(NativeProperty::WhileListConstants) && enabled_)
            return true;
    }
    return false;
}
