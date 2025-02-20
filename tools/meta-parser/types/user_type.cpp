#include "user_type.hpp"
#include <cassert>
#include <format>
#include <utility>
#include "constant.hpp"
#include "field.hpp"
#include "function.hpp"
#include "utilities/clang_utils.hpp"

std::unordered_map<unsigned, LanguageType *> LanguageType::all_types_;

void LanguageType::Clean ()
{
    for (auto &x: all_types_)
    {
        delete x.second;
    }
    all_types_.clear ();
}

std::string LanguageType::GetTypeFullName (unsigned hash)
{
    auto it = all_types_.find (hash);
    if (it != end (all_types_))
        return it->second->FullName ();
    return "";
}

LanguageType::LanguageType (std::string name, const Namespace &ns, TypeInfo *parent, const Cursor &cursor,
                            std::list<LanguageType *> &type_container) : TypeInfo (cursor, ns, parent), container_ (type_container)
{
    all_types_[clang_hashCursor (cursor)] = this;
    // Print(cursor, false);
    name_ = std::move (name);
    // std::cout << " " << FullName() << '\n';
    if (!ShouldCompile ())
        return;
    assert (cursor.IsUserType ());
    enum_ = cursor.IsEnumType ();
    anon_ = cursor.IsAnonymous ();
    if (enum_)
        ParseEnumType (cursor);
    else
        ParseDataType (cursor);
}
LanguageType::~LanguageType ()
{
    for (auto &x: fields_)
        delete x;
    for (auto &x: functions_)
        delete x;
    for (auto &x: constants_)
        delete x;
}
void LanguageType::ParseDataType (const Cursor &cursor)
{
    // TODO: get fields and functions
    auto ns = namespace_;
    if (!anon_)
        ns.Push (name_);
    auto children = cursor.Children ();
    for (auto it = children.begin (); it != children.end (); ++it)
    {
        // Print(*it);
        auto const kind = it->Kind ();
        if (kind == CXCursor_CXXBaseSpecifier)
        {
            auto base = clang_hashCursor (clang_getCursorDefinition (*it));
            // std::cout << "base class " << GetTypeFullName (base) << "\n";
            auto basename = GetTypeFullName(base);
            if (!basename.empty())
                base_types_.emplace_back(std::move(basename));
        }
        else if (kind == CXCursor_FieldDecl || kind == CXCursor_VarDecl)
        {
            if (it->Type ().IsReferenced ())
                continue;
            auto f = new Field (*it, ns, this);
            if (f->ShouldCompile ())
                fields_.emplace_back (f);
            else
                delete f;
        }
        else if (kind == CXCursor_Constructor)
        {
            // TODO: add constructors here
            Print(*it);
        }
        else if (kind == CXCursor_CXXMethod)
        {
            auto name = it->DisplayName ();
            if (name.find ("operator") == 0) // not allow operators
                continue;
            auto f = new Function (*it, ns, this);
            if (f->ShouldCompile ())
                functions_.emplace_back (f);
            else
                delete f;
        }
        else if (kind == CXCursor_EnumDecl)
        {
            if (it->IsAnonymous ())
            {
                auto const decl = clang_getCursorLocation (*it);
                auto const bak  = it;
                // check next field decl is this type
                while (it != end (children) && it->Kind () != CXCursor_FieldDecl)
                    ++it;
                if (it != end (children))
                {
                    // check item type
                    auto type      = clang_getCursorType (*it);
                    auto type_decl = clang_getCursorLocation (clang_getTypeDeclaration (type));
                    if (clang_equalLocations (decl, type_decl))
                    {
                        if (anon_)
                        {
                            it = bak;  // restore
                            continue;  // anonymous in anonymous was not supported
                        }

                        // parse a new type
                        std::string name      = std::format ("Enum_{:X}", clang_hashCursor (*bak));
                        std::string qualifier = std::format ("decltype({})", ns.GetFullQualifiedName (it->DisplayName ()));
                        Namespace   custom;
                        custom.Push (qualifier);
                        auto &r         = container_.emplace_back (new LanguageType {std::move (name), custom, this, *bak, container_});
                        r->custom_name_ = qualifier;
                        if (!container_.back ()->ShouldCompile ())
                            container_.pop_back ();
                    }
                    else
                    {
                        // directly parse into this type
                        ParseEnumType (*bak);
                    }
                }
                else
                {
                    // directly parse into this type
                    ParseEnumType (*bak);
                }
                it = bak;  // restore iter
            }
            else
            {
                container_.emplace_back (new LanguageType {it->DisplayName (), ns, this, *it, container_});
                if (!container_.back ()->ShouldCompile ())
                    container_.pop_back ();
            }
        }
        else if (kind == CXCursor_StructDecl || kind == CXCursor_UnionDecl)
        {
            if (it->IsAnonymous ())
            {
                auto const decl = clang_getCursorLocation (*it);
                auto const bak  = it;
                // check next field decl is this type
                while (it != end (children) && it->Kind () != CXCursor_FieldDecl)
                    ++it;
                if (it != end (children))
                {
                    // check item type
                    auto type      = clang_getCursorType (*it);
                    auto type_decl = clang_getCursorLocation (clang_getTypeDeclaration (type));
                    if (clang_equalLocations (decl, type_decl))
                    {
                        if (anon_)
                        {
                            it = bak;  // restore
                            continue;  // anonymous in anonymous was not supported
                        }

                        // parse a new type
                        std::string name      = std::format ("Type_{:X}", clang_hashCursor (*bak));
                        std::string qualifier = std::format ("decltype({})", ns.GetFullQualifiedName (it->DisplayName ()));
                        Namespace   custom;
                        custom.Push (qualifier);
                        auto &r         = container_.emplace_back (new LanguageType {std::move (name), custom, this, *bak, container_});
                        r->custom_name_ = qualifier;
                        if (!container_.back ()->ShouldCompile ())
                            container_.pop_back ();
                    }
                    else
                    {
                        // directly parse into this type
                        ParseDataType (*bak);
                    }
                }
                else
                {
                    // directly parse into this type
                    ParseEnumType (*bak);
                }
                it = bak;  // restore iter
            }
            else
            {
                container_.emplace_back (new LanguageType {it->DisplayName (), ns, this, *it, container_});
                if (!container_.back ()->ShouldCompile ())
                    container_.pop_back ();
            }
        }
    }
}
void LanguageType::ParseEnumType (const Cursor &cursor)
{
    // TODO: get enums
    auto ns = namespace_;
    ns.Push (name_);
    auto children = cursor.Children ();
    for (auto &x: children)
    {
        if (x.Kind () == CXCursor_EnumConstantDecl)
        {
            auto c = new Constant (x, ns, this);
            if (c->ShouldCompile ())
                constants_.emplace_back (new Constant (x, ns, this));
            else
                delete c;
        }
    }
}

std::string LanguageType::FullName () const
{
    if (!custom_name_.empty ())
        return custom_name_;
    return namespace_.GetFullQualifiedName (name_);
}
std::string LanguageType::Name () const
{
    return name_;
}
bool LanguageType::IsEnum () const
{
    return enum_;
}
bool LanguageType::ShouldCompile () const
{
    return parent_ ? parent_->ShouldCompile () : enabled_;
}
const std::list<std::string> &LanguageType::BaseTypes () const
{
    return base_types_;
}
const std::list<Field *> &LanguageType::Fields () const
{
    return fields_;
}
const std::list<Function *> &LanguageType::Functions () const
{
    return functions_;
}
const std::list<Constant *> &LanguageType::Constants () const
{
    return constants_;
}
