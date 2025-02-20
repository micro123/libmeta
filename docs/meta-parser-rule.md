# CodeGen for libmeta

## Rules
- UserTypes (class or struct)
  - Fields
    - All non-static members, include const member, **UNLESS** marked with `Disabled`
    - Non-const static members, **UNLESS** marked with `Disabled`
    - Explicit marked with `Enabled` const static member
  - Methods
    - All public member/static, **UNLESS** marked with `Disabled`
    - Non-public **BUT** marked with `Enabled`
- Enum Types
  - Items
    - All, **UNLESS** marked with `Disabled`

## META 
- Add `Enabled` by default
- Can add `Disabled` to explicit `Disable` item
- `Property` will add extra infomations (to be implement)

## REFL_BODY(TypeName)

If you get member/type access error (like protected or private), try adding this macro inside class/struct.

## Note
- Anonymous types in **GLOBAL** namespace was **NOT** supported
- Nested anonymous types are also **NOT** supported
