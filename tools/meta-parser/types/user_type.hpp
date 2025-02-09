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
    LanguageType (const Namespace &ns, std::string name, const Cursor &cursor);
    ~LanguageType () override;

    [[nodiscard]] std::string FullName() const;
    [[nodiscard]] std::string Name() const;

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
    std::list<Field *>    fields_;
    std::list<Function *> functions_;
    std::list<Constant *> constants_;
};

#endif /* USER_TYPE_HPP */
