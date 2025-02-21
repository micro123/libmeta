#include "meta_info.hpp"
#include "utilities/str_utils.hpp"
#include <utilities/clang_utils.hpp>
MetaInfo::MetaInfo (const Cursor &cursor)
{
    const auto children = cursor.Children ();
    for (auto child: children)
    {
        if (child.Kind () != CXCursor_AnnotateAttr)
        {
            continue;
        }

        for (auto &[fst, snd]: ExtractProperties (child))
            properties_[fst] = snd;
    }
}
MetaInfo::~MetaInfo () = default;
// bool MetaInfo::GetFlag (const std::string &key) const
// {
//     return properties_.contains (key);
// }
bool MetaInfo::IsEnabled () const
{
    return !IsDisabled() && properties_.contains(NativeProperty::Enabled);
}
bool MetaInfo::IsDisabled () const
{
    return properties_.contains(NativeProperty::Disabled);
}
std::string MetaInfo::GetProperty (const std::string &key) const
{
    const auto it = properties_.find (key);
    return it != properties_.end () ? it->second : std::string ();
}
std::vector<MetaInfo::Property> MetaInfo::ExtractProperties (const Cursor &anno)
{
    std::vector<Property> ret;

    const auto displayName = anno.DisplayName ();
    if (displayName.empty())
        return ret;

    // Print(anno);

    auto properties = split (displayName, ",");

    static const std::string white_space_string = " \t\r\n";

    for (auto &property_item: properties)
    {
        std::string key, value;
        auto pos = property_item.find(":");
        if (pos != std::string::npos)
        {
            key = property_item.substr(0, pos);
            value = property_item.substr(pos+1);
        }
        else
        {
            key = property_item;
            value = "";
        }
        trim(key, white_space_string);
        trim(value, white_space_string);

        if (!key.empty()) {
            ret.emplace_back(key, value);
        }
    }

    return ret;
}
