#include "serialization/xml.hpp"
#include <refl/any.hpp>
#include <refl/field.hpp>
#include "pugixml/pugixml.hpp"

#include <sstream>

using namespace pugi;

static void ToXml(const Meta::Any &value, xml_node &node)
{
    auto type = value.Type();
    if (!type)
        return;
    auto fields = type->GetFields();
    if (fields.empty())
    {
        auto s = value.Value<Meta::str>();
        node.text().set(s.c_str());
    }
    else {
        for (auto &x: fields) {
            if (x->IsMember()) {
                auto child = node.append_child(x->Name());
                // child.append_attribute("type").set_value(x->Type()->Name());
                ToXml(x->Get(&value), child);
            }
        }
    }
}

Meta::str Meta::XmlSerialize(const Meta::Any& value, bool formatted)
{
    xml_document doc;
    auto root = doc.append_child("content");
    ToXml(value, root);
    std::ostringstream oss;
    doc.save(oss, "  ", formatted ? format_indent : format_raw, encoding_utf8);
    return oss.str();
}

static bool FromXml(const Meta::Any &obj, const xml_node& node)
{
    auto type = obj.Type();
    if (!type)
        return true;
    auto fields = type->GetFields();
    if (fields.empty()) {
        return type->FromString(obj, node.text().as_string());
    }
    bool ok = true;
    for (auto &x: fields) {
        if (x->IsMember()) {
            if (!FromXml(x->Get(&obj), node.child(x->Name())))
            {
                ok = false;
                break;
            }
        }
    }
    return ok;
}

bool Meta::XmlDeserialize(const Meta::Any &obj, const Meta::str& value)
{
    xml_document doc;
    if (auto ret = doc.load_string(value.c_str()))
    {
        auto root = doc.document_element();
        return FromXml(obj, root);
    }
    return false;
}
