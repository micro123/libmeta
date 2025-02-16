#include "type_info.hpp"

#include <utility>

TypeInfo::TypeInfo (const Cursor &cursor, Namespace namespace_, TypeInfo *parent): meta_info_ (cursor), namespace_(std::move(namespace_)), parent_ (parent), cursor_(cursor)
{
    enabled_ = meta_info_.GetFlag (NativeProperty::Enabled);
}
TypeInfo::~TypeInfo () = default;
Namespace       TypeInfo::GetNamespace () const
{
    return namespace_;
}
Cursor          TypeInfo::GetCursor () const
{
    return cursor_;
}
std::string     TypeInfo::GetSourceFile () const
{
    return cursor_.SourceFile ();
}
const MetaInfo &TypeInfo::GetMetaInfo () const
{
    return meta_info_;
}
bool TypeInfo::ShouldCompile () const
{
    return enabled_;
}
