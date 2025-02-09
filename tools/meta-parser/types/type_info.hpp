#ifndef TYPE_INFO_HPP
#define TYPE_INFO_HPP

#include <clang/cursor.hpp>
#include "namespace.hpp"
#include "parser/meta_info.hpp"

class TypeInfo
{
public:
    TypeInfo (const Cursor &cursor, Namespace namespace_, TypeInfo *parent = nullptr);
    virtual ~TypeInfo ();
    Namespace       GetNamespace () const;
    Cursor          GetCursor () const;
    std::string     GetSourceFile () const;
    const MetaInfo &GetMetaInfo () const;

    virtual bool ShouldCompile () const;
protected:
    MetaInfo   meta_info_;
    bool       enabled_;
    Namespace  namespace_;
    TypeInfo  *parent_;
private:
    Cursor    cursor_;
};

#endif  // TYPE_INFO_HPP
