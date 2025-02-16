#ifndef LIBMETA_BUILDERS_HPP
#define LIBMETA_BUILDERS_HPP

#include <utility/constexpr.hpp>
#include "exports.h"
#include "refl/fwd.hpp"
#include "refl/generic_type.hpp"
#include "refl/field.hpp"
#include "refl/method.hpp"

namespace Meta {
    namespace details
    {
        template <typename F>
        struct MethodTraits;

        template <typename R, typename ... P>
        struct MethodTraits<R(*)(P...)>
        {
            using ParamTuple = std::tuple<P...>;
        };

        template <typename R, typename C, typename ... P>
        struct MethodTraits<R(C::*)(P...)>
        {
            using ParamTuple = std::tuple<P...>;
        };

        template <typename R, typename C, typename ... P>
        struct MethodTraits<R(C::*)(P...) const>
        {
            using ParamTuple = std::tuple<P...>;
        };

        template <typename R, typename C, typename ... P>
        struct MethodTraits<R(C::*)(P...) volatile>
        {
            using ParamTuple = std::tuple<P...>;
        };

        template <typename F, size_t N>
        struct MethodParamType
        {
            using ParamTuple = typename MethodTraits<F>::ParamTuple;
            using Type = std::tuple_element_t<N, ParamTuple>;
        };
    }

    template <typename F, size_t N>
    using n_param_t = typename details::MethodParamType<F, N>::Type;

#define NParamId(F,N) GetTypeId<n_param_t<decltype(&F), N>>()

    class LIBMETA_API MethodBuilder final
    {
        class PrivateData;
        PrivateData* const d;
    public:
        template <typename F>
        static MethodBuilder NewMethodBuilder(sview name, F f)
        {
            return MakeMethod(name, f);
        }

        MethodBuilder& AddParam(sview name, TypeId type_id, Any def = {});

        [[nodiscard]] MethodPtr Build() const;
        ~MethodBuilder();
    private:
        MethodBuilder(MethodPtr method);
    };

    class LIBMETA_API TypeBuilder final
    {
        class PrivateData;
        PrivateData* const d;
        using MyTypePtr = GenericType::Ptr;

        static Any EnumToString(const Any& value);
        static bool EnumFromString(const Any& obj, const Any& value);
    public:
        template<typename T>
        static TypeBuilder NewTypeBuilder(sview name = {})
        {
            return {
                MakeRef<GenericType>(name.empty() ? GetTypeName<T>() : name, sizeof(T), CalcTypeFlags<T> ()),
                GetTypeId<T>()
            };
        }

        TypeBuilder &AddField(FieldPtr field);
        TypeBuilder &AddConstant(ConstantPtr constant);
        TypeBuilder &AddMethod(MethodPtr method);

        TypeBuilder &AddCastTo(Type::CastPorc proc, const TypeId &type_id);
        TypeBuilder &AddConvertFrom(Type::ConvertProc proc, const TypeId &type_id);

        template<typename Enum>
        TypeBuilder &AddEnumOperations()
        {
            static_assert(std::is_enum_v<Enum>, "Enum type must be an enum");
            using UT = std::underlying_type_t<Enum>;

            // For int types
#define TO(x) +[](const Any &v)->Any { return (x)static_cast<UT>(v.ValueRef<Enum>()); }
#define CAST_TO(x) AddCastTo(TO(x), GetTypeId<x>())
            INT_TYPES (CAST_TO);
#define FROM(x) +[](const Any &obj, const Any &v)->bool { obj.ValueRef<Enum>() = static_cast<Enum>((UT)v.ValueRef<x>()); return true; }
#define CAST_FROM(x) AddConvertFrom(FROM(x), GetTypeId<x>())
            INT_TYPES (CAST_FROM);
#undef TO
#undef CAST_TO
#undef FROM
#undef CAST_FROM

            // For string
            AddCastTo (&EnumToString, GetTypeId<str>());
            AddConvertFrom (&EnumFromString, GetTypeId<str>());
            return *this;
        }

        [[nodiscard]] TypePtr Register(TypeId tid = NULL_TYPE_ID) const;
        ~TypeBuilder();
    private:
        TypeBuilder(MyTypePtr type, const TypeId& tid);
    };
}

#endif //LIBMETA_BUILDERS_HPP
