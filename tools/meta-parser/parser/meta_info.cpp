#include "meta_info.hpp"
#include "utilities/str_utils.hpp"
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
bool MetaInfo::GetFlag (const std::string &key) const
{
    return properties_.contains (key);
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

    auto properties = split (displayName, ",");

    static const std::string white_space_string = " \t\r\n";

    for (auto &property_item: properties)
    {
        auto &&item_details = split (property_item, ":");
        auto &&temp_string  = trim (item_details[0], white_space_string);
        if (temp_string.empty ())
        {
            continue;
        }
        ret.emplace_back (temp_string, item_details.size () > 1 ? trim (item_details[1], white_space_string) : "");
    }

    return ret;
}
