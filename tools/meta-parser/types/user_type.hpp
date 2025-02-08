#ifndef USER_TYPE_HPP
#define USER_TYPE_HPP

#include <list>
#include "clang/cursor.hpp"

class Field;
class Function;
class Constant;

class LanguageType final
{
public:
    LanguageType (const Cursor &cursor);
    ~LanguageType ();

private:
    // 常规解析，用于结构体/联合体
    void ParseDataType();
    // 枚举解析，用于枚举
    void ParseEnumType();

    std::string           fullname_;
    Cursor                cursor_;
    std::list<Field *>    fields_;
    std::list<Function *> functions_;
    std::list<Constant *> constants_;
};

#endif /* USER_TYPE_HPP */
