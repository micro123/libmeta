#ifndef TYPE_INFO_HPP
#define TYPE_INFO_HPP

#include <clang/cursor.hpp>
#include <list>
#include "namespace.hpp"
#include "parser/meta_info.hpp"

struct CustomProperty {
    std::string name;
    std::string init;
};

using PropList = std::list<CustomProperty>;

class TypeInfo
{
public:
    TypeInfo (const Cursor &cursor, Namespace namespace_, TypeInfo *parent = nullptr);
    virtual ~TypeInfo ();
    Namespace       GetNamespace () const;
    Cursor          GetCursor () const;
    std::string     GetSourceFile () const;
    const MetaInfo &GetMetaInfo () const;
    const PropList &GetCustomProperties () const;

    [[nodiscard]] bool HasProperties () const
    {
        return !custom_properties_.empty ();
    }

    virtual bool ShouldCompile () const;

protected:
    MetaInfo  meta_info_;
    bool      enabled_;
    Namespace namespace_;
    TypeInfo *parent_;

private:
    Cursor   cursor_;
    PropList custom_properties_;
};

#endif  // TYPE_INFO_HPP
