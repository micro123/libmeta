#ifndef LIBMETA_TYPE_INL
#define LIBMETA_TYPE_INL

#include <type_traits>

#define FUNDAMENTAL_TYPES(X) \
    X (s8);                  \
    X (s16);                 \
    X (s32);                 \
    X (s64);                 \
    X (u8);                  \
    X (u16);                 \
    X (u32);                 \
    X (u64);                 \
    X (f32);                 \
    X (f64);                 \
    X (bool);                \
    X (char)

#define LINKAGE_TYPEID(type) \
    template <>              \
    TypeId LIBMETA_API details::GetTypeIdImpl<type> ()

namespace Meta
{
    FUNDAMENTAL_TYPES (LINKAGE_TYPEID);

    // void type
    LINKAGE_TYPEID(void);
    // str
    LINKAGE_TYPEID(str);
    // const char *
    template <>
    TypeId LIBMETA_API GetTypeId<cstr> ();
}

#undef LINKAGE_TYPEID

namespace Meta
{
    namespace details
    {
        template <bool v, u32 f>
        struct cond_value {
            static constexpr u32 value = v ? f : 0;
        };

        template <typename T>
        constexpr u32 f_pod = cond_value<std::is_trivial_v<std::remove_all_extents_t<T>>, eTypeIsPod>::value;

        template <typename T>
        constexpr u32 f_const = cond_value<std::is_const_v<T>, eTypeIsConst>::value;

        template <typename T>
        constexpr u32 f_ref = cond_value<std::is_reference_v<T>, eTypeIsRef>::value;

        template <typename T>
        constexpr u32 f_ptr = cond_value<std::is_pointer_v<T>, eTypeIsPtr>::value;

        template <typename T>
        constexpr u32 f_virtual = cond_value<std::is_abstract_v<T>, eTypeIsVirtual>::value;
    }  // namespace details

    template <typename T>
    constexpr u32 CalcTypeFlags ()
    {
        using namespace details;
        return f_pod<T> + f_const<T> + f_ptr<T> + f_ref<T> + f_virtual<T>;
    }
}  // namespace Meta

#endif /* LIBMETA_TYPE_INL */
