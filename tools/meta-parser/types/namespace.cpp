#include "namespace.hpp"

void        Namespace::Push (std::string ns_or_type)
{
    ns_stack_.emplace_back (std::move(ns_or_type));
}
void        Namespace::Pop ()
{
    ns_stack_.pop_back ();
}
std::string Namespace::GetFullQualifiedName (std::string ns_or_type) const
{
    std::string result;
    for (auto &x: ns_stack_)
    {
        if (x.empty ())
            continue;
        if (!x.starts_with("decltype"))
            result.append ("::");
        result.append(x);
    }
    if (!ns_or_type.empty ())
        result.append ("::").append (ns_or_type);
    return result;
}
