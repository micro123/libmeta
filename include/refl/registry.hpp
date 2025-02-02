#ifndef REGISTRY_HPP
#define REGISTRY_HPP

#include "fwd.hpp"
#include <functional>

namespace Meta
{
    class LIBMETA_API Registry
    {
        Registry ();
        ~Registry ();

    public:
        static Registry &Instance ();

        bool RegisterType (TypePtr type, TypeId tid);

        TypePtr Get (TypeId tid);

        TypePtr Get (const str name);

        void VisitTypes(const std::function<bool(TypeId, TypePtr)> &visitor) const;
        
    private:
        class Private;
        Private *const d;
    };
}  // namespace Meta

#endif /* REGISTRY_HPP */
