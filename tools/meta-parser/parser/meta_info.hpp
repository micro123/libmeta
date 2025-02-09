#ifndef META_INFO_HPP
#define META_INFO_HPP

#include <string>
#include <unordered_map>
#include <vector>
#include "clang/cursor.hpp"

namespace NativeProperty
{
#define PROP(name) constexpr auto name = #name
    PROP (All);

    PROP (Fields);
    PROP (Methods);
    PROP (Constants);

    PROP (Enabled);
    PROP (Disabled);

    PROP (WhileListFields);
    PROP (WhileListMethods);
    PROP (WhileListConstants);
#undef PROP
}

class MetaInfo
{
public:
    MetaInfo(const Cursor &cursor);
    ~MetaInfo();

    bool GetFlag(const std::string &key) const;
    std::string GetProperty(const std::string &key) const;
private:
    std::unordered_map<std::string, std::string> properties_;
    using Property = decltype(properties_)::value_type;

    static std::vector<Property> ExtractProperties(const Cursor &anno);
};

#endif  // META_INFO_HPP
