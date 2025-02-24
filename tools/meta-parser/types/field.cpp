#include "field.hpp"
#include <cassert>
Field::Field (const Cursor &cursor, const Namespace &ns, TypeInfo *parent): TypeInfo (cursor, ns, parent)
{
    assert(parent);
    name_ = cursor.DisplayName ();
    is_bit_field_ = cursor.IsBitField();
    if (is_bit_field_) {
        offset_ = clang_Cursor_getOffsetOfField(cursor);
        bits_ = clang_getFieldDeclBitWidth(cursor);
    } else {
        offset_ = bits_ = 0;
    }
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
