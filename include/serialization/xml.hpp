#ifndef LIBMETA_XML_HPP
#define LIBMETA_XML_HPP

#include "refl/fwd.hpp"
#include <string>

namespace Meta
{
    class Any;

    LIBMETA_API
    str XmlSerialize(const Any& value, bool formatted = false);

    LIBMETA_API
    bool XmlDeserialize(const Any &obj, const str& value);
}

#endif /* LIBMETA_XML_HPP */
