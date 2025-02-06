//
// Created by tang on 25-2-6.
//

#ifndef LIBMETA_CONSTANT_HPP
#define LIBMETA_CONSTANT_HPP

#include "refl/fwd.hpp"
#include "refl/any.hpp"

namespace Meta {
    class LIBMETA_API Constant
    {
    public:
        Constant(sview name, Any value);
        ~Constant();

        sview Name() const;
        Any Value() const;
    private:
        sview const name_;
        Any   const value_;
    };
}

#endif //LIBMETA_CONSTANT_HPP
