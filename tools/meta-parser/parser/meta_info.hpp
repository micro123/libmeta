#ifndef META_INFO_HPP
#define META_INFO_HPP

#include <string>
#include <unordered_map>
#include <vector>
#include "clang/cursor.hpp"

namespace NativeProperty
{
#define PROP(name) constexpr auto name = #name
    PROP (Enabled);
    PROP (Disabled);
    PROP (Property);
#undef PROP
}

class MetaInfo
{
public:
    MetaInfo(const Cursor &cursor);
    ~MetaInfo();

    bool IsEnabled() const;
    bool IsDisabled() const;
    std::string GetProperty(const std::string &key) const;
private:
    std::unordered_map<std::string, std::string> properties_;
    using Property = decltype(properties_)::value_type;

    static std::vector<Property> ExtractProperties(const Cursor &anno);
};

#endif  // META_INFO_HPP
