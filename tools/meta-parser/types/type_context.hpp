#ifndef TYPE_CONTEXT_HPP
#define TYPE_CONTEXT_HPP

#include "type_info.hpp"

class LanguageType;
class TypeContext final : public TypeInfo
{
public:
    TypeContext (const Cursor &cursor, Namespace namespace_);
    ~TypeContext ();

    const std::list<LanguageType> &GetTypes () const;
    std::string GetName () const;
    void VisitAst(const Cursor &cursor);
    bool ShouldCompile() const override;

private:
    std::string             class_name_;
    std::list<LanguageType> type_list_;
};

#endif  // TYPE_CONTEXT_HPP
