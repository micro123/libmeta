#ifndef USER_TYPE_HPP
#define USER_TYPE_HPP

#include <list>
#include <string>

#include "clang/cursor.hpp"
#include "types/type_info.hpp"

class Field;
class Function;
class Constant;

class LanguageType final : public TypeInfo
{
public:
    LanguageType (std::string name, const Namespace &ns, TypeInfo *parent, const Cursor &cursor, std::list<LanguageType> &type_container);
    ~LanguageType () override;

    [[nodiscard]] std::string FullName() const;
    [[nodiscard]] std::string Name() const;
    [[nodiscard]] bool IsEnum() const;

    const std::list<Field *> &Fields() const;
    const std::list<Function *> &Functions() const;
    const std::list<Constant *> &Constants() const;

    bool ShouldCompile() const override;
private:
    // 常规解析，用于结构体/联合体
    void ParseDataType(const Cursor &cursor);
    // 枚举解析，用于枚举
    void ParseEnumType(const Cursor &cursor);

    std::string           name_;
    std::string           custom_name_;
    bool                  enum_;
    bool                  anno_;
    std::list<Field *>    fields_;
    std::list<Function *> functions_;
    std::list<Constant *> constants_;
    std::list<LanguageType> &container_;
};

#endif /* USER_TYPE_HPP */
