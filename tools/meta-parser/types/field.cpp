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
    return true;
}
