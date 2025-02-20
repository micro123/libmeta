#include "cursor_type.hpp"
#include "utilities/clang_utils.hpp"

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

unsigned    CursorType::GetNumArguments () const
{
    return clang_getNumArgTypes(type_);
}

std::string CursorType::Spelling () const
{
    return ToString (clang_getTypeSpelling(type_));
}
