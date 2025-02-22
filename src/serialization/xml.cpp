#include "serialization/xml.hpp"
#include <refl/any.hpp>
#include <refl/field.hpp>
#include <sstream>
#include "pugixml/pugixml.hpp"

using namespace pugi;

static void ToXml (const Meta::Any &value, xml_node &node)
{
    auto type = value.Type ();
    if (!type)
        return;
    node.append_attribute("type").set_value(type->Name());    
    auto type_ids = type->GetBaseTypeIds();
    for (auto const &id: type_ids) {
        Meta::Any temp;
        if (Meta::AnyCast(value, temp, id))
        {
            auto base_type = temp.Type();
            if (!base_type)
                continue;
            auto n = node.append_child(base_type->Name());
            ToXml(temp, n);
        }
    }
    auto fields = type->GetFields ();
    if (fields.empty ())
    {
        auto s = value.Value<Meta::str> ();
        node.text ().set (s.c_str ());
    }
    else
    {
        for (auto &x: fields)
        {
            if (x->IsMember ())
            {
                auto const cnt = x->Count ();
                if (cnt <= 1)
                {
                    auto child = node.append_child (x->Name ());
                    ToXml (x->Get (&value), child);
                }
                else
                {
                    auto array = node.append_child (x->Name ());
                    array.append_attribute ("type").set_value (x->Type ()->Name ());
                    array.append_attribute ("count").set_value (cnt);
                    for (u32 i = 0; i < cnt; ++i)
                    {
                        auto item = array.append_child ("item");
                        ToXml (x->Get (&value, i), item);
                    }
                }
            }
        }
    }
}

Meta::str Meta::XmlSerialize (const Meta::Any &value, bool formatted)
{
    xml_document doc;
    auto         root = doc.append_child ("root");
    ToXml (value, root);
    std::ostringstream oss;
    doc.save (oss, "  ", formatted ? format_indent : format_raw, encoding_utf8);
    return oss.str ();
}

static bool FromXml (const Meta::Any &obj, const xml_node &node)
{
    auto type = obj.Type ();
    if (!type)
        return true;
    auto type_ids = type->GetBaseTypeIds();
    for (auto const &id: type_ids) {
        Meta::Any temp;
        if (Meta::AnyCast(obj, temp, id))
        {
            auto base_type = temp.Type();
            if (!base_type)
                continue;
            auto n = node.child(base_type->Name());
            if (n)
                FromXml(temp, n);
        }
    }
    auto fields = type->GetFields ();
    if (fields.empty ())
    {
        return type->ValueFromString (obj, node.text ().as_string ());
    }
    bool ok = true;
    for (auto &x: fields)
    {
        if (x->IsMember ())
        {
            auto const cnt = x->Count ();
            if (cnt <= 1)
            {
                if (!FromXml (x->Get (&obj), node.child (x->Name ())))
                {
                    ok = false;
                    break;
                }
            }
            else
            {
                auto array = node.child (x->Name ());
                auto item  = array.child ("item");
                for (u32 i = 0; i < cnt; ++i)
                {
                    if (!FromXml (x->Get (&obj, i), item))
                    {
                        ok = false;
                        break;
                    }
                    item = item.next_sibling ("item");
                }
            }
        }
        if (!ok)
            break;
    }
    return ok;
}

bool Meta::XmlDeserialize (const Meta::Any &obj, const Meta::str &value)
{
    xml_document doc;
    if (auto ret = doc.load_string (value.c_str ()))
    {
        auto root = doc.document_element ();
        return FromXml (obj, root);
    }
    return false;
}
