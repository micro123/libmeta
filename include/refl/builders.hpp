#ifndef LIBMETA_BUILDERS_HPP
#define LIBMETA_BUILDERS_HPP

#include <utility/constexpr.hpp>
#include "exports.h"
#include "refl/fwd.hpp"
#include "refl/generic_type.hpp"
#include "refl/field.hpp"
#include "refl/method.hpp"
#include <type_traits>

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

    struct Ctor
    {
        template <typename T>
        static void Release(void *p)
        {
            if constexpr (Meta::details::IsDisposable<T>)
            {
                static_cast<T*>(p)->Dispose();
            }
            else
            {
                delete static_cast<T*>(p);
            }
        }

        template <typename T, typename ... Args>
        static Ref<T> Of(Args ... args)
        {
            return {new T(args...), &Ctor::Release<T>};
        }
    };

    template<typename T>
    struct Overloaded;

    template<typename R, typename... Args>
    struct Overloaded<R(Args...)>
    {
        static constexpr auto Of(R(*p)(Args...))
        {
            return p;
        }
        template <typename C>
        static constexpr auto Of(R(C::*p)(Args...))
        {
            return p;
        }
    };

    template<typename R, typename... Args>
    struct Overloaded<R(Args...)const>
    {
        template <typename C>
        static constexpr auto Of(R(C::*p)(Args...)const)
        {
            return p;
        }
    };

#define NParamT(F,N) n_param_t<decltype(&F), N>
#define BaseCvt(T,B) +[](const Meta::Any &in)-> Meta::Any { return static_cast<B*>(in.ValuePtr<T>()); }

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

        MethodBuilder& AddParam(u32 idx, sview name, Any def = {});

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
        struct PropertyItem {
            str name;
            Any value;
        };

        template<typename T>
        static TypeBuilder NewTypeBuilder(sview name = {})
        {
            return {
                MakeRef<GenericType>(GetTypeId<T>(), name.empty() ? GetTypeName<T>() : name, sizeof(T), CalcTypeFlags<T> ()),
                GetTypeId<T>()
            };
        }

        TypeBuilder &AddBaseType(TypeId id, Type::CastProc cast);
        TypeBuilder &AddField(FieldPtr field);
        TypeBuilder &AddConstant(ConstantPtr constant);
        TypeBuilder &AddMethod(MethodPtr method);
        TypeBuilder &AddConstructor(MethodPtr method);

        TypeBuilder &AddCastTo(Type::CastProc proc, const TypeId &type_id);
        TypeBuilder &AddConvertFrom(Type::ConvertProc proc, const TypeId &type_id);

        TypeBuilder &WithProperties(const std::initializer_list<PropertyItem> &properties);

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
            AddConvertFrom (&EnumFromString, GetTypeId<cstr>());
            return *this;
        }

        template <typename T>
        TypeBuilder &AddDefaultConstructor(sview name) {
            if constexpr(std::is_default_constructible_v<T>)
            {
                return AddConstructor(
                    MethodBuilder::NewMethodBuilder (name, &Meta::Ctor::Of<T>)
                    .Build()
                );
            }
            return *this;
        }

        ~TypeBuilder();
    private:
        void Register() const;
        
        TypeBuilder(MyTypePtr type, const TypeId& tid);
    };
}

#endif //LIBMETA_BUILDERS_HPP
