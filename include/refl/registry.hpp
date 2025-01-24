#ifndef LIBMETA_REGISTRY_HPP
#define LIBMETA_REGISTRY_HPP

#include "fwd.hpp"

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

    private:
        class Private;
        Private *const d;
    };
}  // namespace Meta

#endif /* LIBMETA_REGISTRY_HPP */
