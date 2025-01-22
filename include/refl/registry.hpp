#ifndef LIBMETA_REGISTRY_HPP
#define LIBMETA_REGISTRY_HPP

#include "fwd.hpp"

namespace Meta {
    class LIBMETA_API Registry {
        Registry();
        ~Registry();
    public:
        static Registry &Instance();

        bool RegisterType(Type *type, TypeId tid);

        Type *Get(TypeId tid);
    private:
        class Private;
        Private * const d;
    };
}

#endif /* LIBMETA_REGISTRY_HPP */
