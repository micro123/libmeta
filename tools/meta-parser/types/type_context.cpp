#include "type_context.hpp"
#include "user_type.hpp"

TypeContext::TypeContext (const Cursor &cursor, Namespace namespace_): TypeInfo (cursor, namespace_)
{
    class_name_ = namespace_.GetFullQualifiedName (cursor.DisplayName ());
    VisitAst (cursor);
}

TypeContext::~TypeContext () = default;

const std::list<LanguageType> &TypeContext::GetTypes () const
{
    return type_list_;
}

std::string TypeContext::GetName () const
{
    return class_name_;
}

void TypeContext::VisitAst (const Cursor &cursor)
{
    type_list_.emplace_back (cursor.DisplayName (), GetNamespace (), nullptr, cursor, type_list_);
    if (!type_list_.back ().ShouldCompile ())
        type_list_.pop_back ();
}
bool TypeContext::ShouldCompile () const
{
    return TypeInfo::ShouldCompile () && !type_list_.empty ();
}
