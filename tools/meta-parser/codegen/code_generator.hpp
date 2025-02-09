#ifndef CODE_GENERATOR_HPP
#define CODE_GENERATOR_HPP

#include <list>
#include <string>

class LanguageType;
// 负责生成类型注册代码
// 需要 当前头文件、类型列表、输出路径
class TypeCodeGen final
{
    class Private;
    Private * const d;
public:
    TypeCodeGen(std::string header, const std::list<LanguageType> &types, std::string output);
    ~TypeCodeGen();

    bool GenerateCode() const;
};

// 负责生成自动注册代码 __RegisterTypes() 函数
// 需要 所有头文件、所有类型名、输出路径
class AutoRegCodeGen final
{
    class Private;
    Private * const d;
public:
    AutoRegCodeGen(const std::list<std::string>& headers, const std::list<std::string> &types, std::string output);
    ~AutoRegCodeGen();

    bool GenerateCode() const;
};

#endif  // CODE_GENERATOR_HPP
