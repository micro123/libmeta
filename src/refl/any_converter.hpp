#ifndef LIBMETA_ANY_CONVERTER_HPP
#define LIBMETA_ANY_CONVERTER_HPP

#include <string>

namespace Meta {

    template <typename From, typename To>
    struct Converter
    {
        static Any Convert(const Any& in) {
            if constexpr (std::is_convertible_v<To, From>)
            {
                return static_cast<To>(in.ValueRef<From>());
            }
            return {};
        }
    };

    template <typename T>
    struct Converter<T, str> {
        static Any Convert(const Any &in) {
            return std::to_string(in.Value<T>());
        }
    };

    template <>
    struct Converter<cstr, str>
    {
        static Any Convert(const Any& in) {
            return std::string(in.Value<cstr>());
        }
    };

    template <typename T>
    struct Converter<str, T>
    {
        static Any Convert(const Any& in) {
            std::string &value = in.ValueRef<str>();
            if constexpr (std::is_signed_v<T>)
            {
                return static_cast<T> (std::stoll (value, nullptr, 0));
            }
            else if constexpr (std::is_unsigned_v<T>)
            {
                return static_cast<T> (std::stoull (value, nullptr, 0));
            }
            else if constexpr (std::is_floating_point_v<T>)
            {
                return static_cast<T> (std::stold (value, nullptr));
            }
            return {};
        }
    };

    template <typename T>
    struct Converter<cstr, T>
    {
        static Any Convert(const Any& in) {
            cstr& value = in.ValueRef<cstr>();
            if constexpr (std::is_signed_v<T>)
            {
                return static_cast<T> (std::strtoll(value, nullptr, 0));
            }
            else if constexpr (std::is_unsigned_v<T>)
            {
                return static_cast<T> (std::strtoull(value, nullptr, 0));
            }
            else if constexpr (std::is_floating_point_v<T>)
            {
                return static_cast<T> (std::strtod(value, nullptr));
            }
            return {};
        }
    };

}


#endif /* LIBMETA_ANY_CONVERTER_HPP */
