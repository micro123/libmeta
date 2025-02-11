#ifndef LIBMETA_ANY_CONVERTER_HPP
#define LIBMETA_ANY_CONVERTER_HPP

#include <string>

template <typename From, typename To>
struct Converter {};

template <typename T>
struct Converter<T, std::string> {
    static Any Convert(const Any &in) {
        return std::to_string(in.Value<T>());
    }
};


#endif /* LIBMETA_ANY_CONVERTER_HPP */
