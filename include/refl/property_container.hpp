#ifndef LIBMETA_PROPERTY_CONTAINER_HPP
#define LIBMETA_PROPERTY_CONTAINER_HPP

#include "refl/any.hpp"
#include <map>

namespace Meta {
    class LIBMETA_API PropertyContainer {
        friend class TypeBuilder;
    public:
        PropertyContainer();
        ~PropertyContainer() = default;
        Any GetProperty(str key) const;
        Any SetProperty(str key, Any value);
    private:
        void AppendProperty(str key, Any value);
        std::map<str, Any> properties_;
    };

    namespace Property {
        // predefined propety types
        
    }
}

#endif /* LIBMETA_PROPERTY_CONTAINER_HPP */
