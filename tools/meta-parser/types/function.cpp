#include "function.hpp"
Function::Function (const Cursor &cursor, const Namespace &ns, TypeInfo *parent) : TypeInfo (cursor, ns, parent)
{
    name_ = cursor.Spelling ();
    // 遍历子节点，获取函数参数
    auto children = cursor.Children ();
    for (auto child : children)
    {
        if (child.Kind () == CXCursor_ParmDecl)
        {
            parameters_.emplace_back (child.Spelling ());
        }
    }
}
Function::~Function () = default;
bool Function::ShouldCompile () const
{
    if (parent_->ShouldCompile())
    {
        auto &pm = parent_->GetMetaInfo();
        auto const disabled_ = meta_info_.GetFlag(NativeProperty::Disabled);
        if ((pm.GetFlag(NativeProperty::All) || pm.GetFlag(NativeProperty::Methods)) && !disabled_)
            return true;
        if (pm.GetFlag(NativeProperty::WhileListMethods) && enabled_)
            return true;
    }
    return false;
}
