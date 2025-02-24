#ifndef FIELD_HPP
#define FIELD_HPP
#include <string>
#include "clang/cursor.hpp"
#include "namespace.hpp"
#include "type_info.hpp"

class Field final: public TypeInfo
{
public:
    Field(const Cursor &cursor, const Namespace &ns, TypeInfo *parent);
    ~Field() override;

    std::string Name() const { return name_; }
    bool IsBitField() const { return is_bit_field_; }
    unsigned Offset() const { return offset_; }
    unsigned BitWidth() const { return bits_; }

    bool ShouldCompile() const override;
private:
    std::string name_;
    bool        is_bit_field_;
    unsigned    offset_, bits_;
};

#endif /* FIELD_HPP */
