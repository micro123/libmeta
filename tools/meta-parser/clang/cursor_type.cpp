#include "cursor_type.hpp"

CursorType::CursorType(CXType type): type_(type) {}

CursorType::~CursorType() = default;

bool CursorType::IsReferenced () const
{
    return type_.kind == CXType_LValueReference || type_.kind == CXType_RValueReference;
}

bool CursorType::IsConstQualified () const
{
    return clang_isConstQualifiedType(type_);
}

CXTypeKind CursorType::Kind () const
{
    return type_.kind;
}
