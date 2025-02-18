#include "field.hpp"
#include <cassert>
Field::Field (const Cursor &cursor, const Namespace &ns, TypeInfo *parent): TypeInfo (cursor, ns, parent)
{
    assert(parent);
    name_ = cursor.DisplayName ();
}
Field::~Field () = default;

bool Field::ShouldCompile () const
{
    if (parent_->ShouldCompile())
    {
        auto const cursor = GetCursor();
        bool const is_static_const = cursor.Kind() == CXCursor_VarDecl && cursor.Type().IsConstQualified();
        return !is_static_const || meta_info_.IsEnabled(); // not static const or explicit enabled
    }
    return false;
}
