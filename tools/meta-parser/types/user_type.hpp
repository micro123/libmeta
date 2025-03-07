#ifndef USER_TYPE_HPP
#define USER_TYPE_HPP

#include <list>
#include <string>
#include <unordered_map>
#include "clang/cursor.hpp"
#include "types/type_info.hpp"

class Field;
class Function;
class Constant;

class LanguageType final : public TypeInfo
{
public:
    LanguageType (std::string name, const Namespace &ns, TypeInfo *parent, const Cursor &cursor, std::list<LanguageType *> &type_container);
    ~LanguageType () override;

    [[nodiscard]] std::string FullName () const;
    [[nodiscard]] std::string Name () const;
    [[nodiscard]] bool        IsEnum () const;
    [[nodiscard]] bool        HasDefaultConstructorDefined() const;

    const std::list<std::string> &BaseTypes () const;
    const std::list<Field *>     &Fields () const;
    const std::list<Function *>  &Constructors () const;
    const std::list<Function *>  &Functions () const;
    const std::list<Constant *>  &Constants () const;

    bool ShouldCompile () const override;

    static void Clean ();

    static std::string GetTypeFullName (unsigned hash);

private:
    // 常规解析，用于结构体/联合体
    void ParseDataType (const Cursor &cursor);
    // 枚举解析，用于枚举
    void ParseEnumType (const Cursor &cursor);

    std::string                name_;         // type name
    std::string                custom_name_;  // use custom name instead of Namespace::Type
    bool                       enum_;         // enum type
    bool                       anon_;         // anonymous
    bool                       default_ctor_; // has default constructor defined
    std::list<std::string>     base_types_;   // base class/struct
    std::list<Field *>         fields_;
    std::list<Function *>      constructors_;
    std::list<Function *>      functions_;
    std::list<Constant *>      constants_;
    std::list<LanguageType *> &container_;  // parser result

    // global type store
    static std::unordered_map<unsigned, LanguageType *> all_types_;
};

#endif /* USER_TYPE_HPP */
