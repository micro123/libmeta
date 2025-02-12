#ifndef LIBMETA_CONSTEXPR_HPP
#define LIBMETA_CONSTEXPR_HPP

#include <string_view>

namespace Meta
{
    // MARK: Get Type's Name, e.g. int, float, class foo, enum bar
    template <typename T>
    constexpr std::string_view GetTypeName ();

    template <>
    constexpr std::string_view GetTypeName<void> ()
    {
        return "void";
    }

    namespace details
    {
        template <typename T>
        constexpr std::string_view WrappedFunctionName ()
        {
#if defined(_MSC_VER)
            return __FUNCSIG__;
#elif defined(__GNUC__)
            return __PRETTY_FUNCTION__;
#elif defined(__clang__)
            return __PRETTY_FUNCTION__;
#else
#error "Unsupported compiler!"
#endif
        }

        using probe_type = void;

        constexpr auto PrefixLength ()
        {
            constexpr auto fname      = WrappedFunctionName<probe_type> ();
            constexpr auto probe_name = GetTypeName<probe_type> ();
            return fname.find (probe_name);
        }

        constexpr auto SuffixLength ()
        {
            constexpr auto fname      = WrappedFunctionName<probe_type> ();
            constexpr auto probe_name = GetTypeName<probe_type> ();
            return fname.length () - PrefixLength () - probe_name.length ();
        }

        template <typename T>
        constexpr std::string_view GetTypeNameImpl ()
        {
            constexpr auto fname  = WrappedFunctionName<T> ();
            constexpr auto pos    = PrefixLength ();
#ifdef _MSC_VER
            constexpr auto space = fname.find(' ', pos);
            if constexpr (space - pos <= 6) // enum  class struct union
            {
                constexpr auto length = fname.length() - space - 1 - SuffixLength();
                return fname.substr(space + 1, length);
            }
#endif
            constexpr auto length = fname.length () - pos - SuffixLength ();
            return fname.substr (pos, length);
        }
    }  // namespace details

    template <typename T>
    constexpr std::string_view GetTypeName ()
    {
        return details::GetTypeNameImpl<T> ();
    }

}  // namespace Meta

#endif /* LIBMETA_CONSTEXPR_HPP */
