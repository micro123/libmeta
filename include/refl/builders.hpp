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

        [[nodiscard]] TypePtr Register(TypeId tid = NULL_TYPE_ID) const;
        ~TypeBuilder();
    private:
        TypeBuilder(MyTypePtr type, TypeId tid);
    };
}

#endif //LIBMETA_BUILDERS_HPP
