#ifndef FUNCTION_HPP
#define FUNCTION_HPP

#include <string>
#include "clang/cursor.hpp"
#include "namespace.hpp"
#include "type_info.hpp"

struct Parameter {
    std::string name;
    std::string init_value;
};

class Function final : public TypeInfo
{
public:
    Function (const Cursor &cursor, const Namespace &ns, TypeInfo *parent);
    ~Function () override;

    std::string Name () const
    {
        return name_;
    }
    std::string Type() const
    {
        return func_type_;
    }
    bool NoArgCallable() const;

    const std::vector<Parameter>& Arguments () const { return parameters_; }

    bool ShouldCompile () const override;

private:
    std::string            name_;
    std::string            func_type_;
    std::vector<Parameter> parameters_;
};

#endif /* FUNCTION_HPP */
