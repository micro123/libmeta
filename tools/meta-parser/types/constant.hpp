#ifndef CONSTANT_HPP
#define CONSTANT_HPP
#include <cstdint>
#include "namespace.hpp"
#include "type_info.hpp"

class Constant final : public TypeInfo
{
public:
    Constant (const Cursor &cursor, const Namespace &ns, TypeInfo *parent);
    ~Constant () override;

    std::string Name () const;
    uint64_t    Value () const;
    bool        ShouldCompile () const override;

private:
    std::string name_;
    int64_t     value_;
};

#endif  // CONSTANT_HPP
