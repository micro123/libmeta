#include "user_type.hpp"
#include <cassert>

#include "field.hpp"
#include "function.hpp"
#include "constant.hpp"

LanguageType::LanguageType (const Cursor &cursor): cursor_(cursor)
{
    assert (cursor_.IsUserType ());
    if (cursor_.IsEnumType ())
        ParseEnumType ();
    else if (cursor_.IsDataType ())
        ParseDataType ();
}
LanguageType::~LanguageType ()
{
    for (auto &x: fields_) delete x;
    for (auto &x: functions_) delete x;
    for (auto &x: constants_) delete x;
}
void LanguageType::ParseDataType ()
{
    // TODO: get fields and functions
}
void LanguageType::ParseEnumType ()
{
    // TODO: get enums
}
