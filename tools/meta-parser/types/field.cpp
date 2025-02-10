#include "field.hpp"
#include <cassert>
Field::Field (const Cursor &cursor, const Namespace &ns, TypeInfo *parent): TypeInfo (cursor, ns, parent)
{
    assert(parent);
    name_ = cursor.DisplayName ();
}
Field::~Field () = default;

bool Field::ShouldCompile () const
{
    if (parent_->ShouldCompile())
    {
        auto &pm = parent_->GetMetaInfo();
        auto const disabled_ = meta_info_.GetFlag(NativeProperty::Disabled);
        if ((pm.GetFlag(NativeProperty::All) || pm.GetFlag(NativeProperty::Fields)) && !disabled_)
            return true;
        if (pm.GetFlag(NativeProperty::WhileListFields) && enabled_)
            return true;
    }
    return false;
}
