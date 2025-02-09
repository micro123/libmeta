#ifndef NAMESPACE_HPP
#define NAMESPACE_HPP

#include <string>
#include <list>

class Namespace
{
public:
    void Push(std::string ns_or_type);
    void Pop();
    [[nodiscard]] std::string GetFullQualifiedName(std::string ns_or_type) const;
private:
    std::list<std::string> ns_stack_;
};

#endif  // NAMESPACE_HPP
