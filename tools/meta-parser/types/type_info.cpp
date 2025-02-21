#include "type_info.hpp"

#include "utilities/str_utils.hpp"

#include <utility>
#include <iostream>

TypeInfo::TypeInfo (const Cursor &cursor, Namespace namespace_, TypeInfo *parent): meta_info_ (cursor), namespace_(std::move(namespace_)), parent_ (parent), cursor_(cursor)
{
    enabled_ = meta_info_.IsEnabled();
    auto properties = meta_info_.GetProperty(NativeProperty::Properties);
    if (!properties.empty()) {
        auto items = split(properties, ";;");
        const std::string white_spaces = " \r\t\n";
        for (auto item: items)
        {
            auto pos = item.find("=");
            if (pos == std::string::npos)
                continue;
            std::string key, val;
            key = trim(item.substr(0,pos), white_spaces);
            val = trim(item.substr(pos+1), white_spaces);
            if (key.empty() || val.empty())
                continue;
            custom_properties_.emplace_back(key, val);
        }
    }
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
const PropList &TypeInfo::GetCustomProperties () const
{
    return custom_properties_;
}
bool TypeInfo::ShouldCompile () const
{
    return enabled_;
}
