#ifndef LIBMETA_BUILDERS_HPP
#define LIBMETA_BUILDERS_HPP

#include <utility/constexpr.hpp>
#include "exports.h"
#include "refl/fwd.hpp"
#include "refl/generic_type.hpp"
#include "refl/field.hpp"
#include "refl/method.hpp"

namespace Meta {
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

        MethodBuilder& AddParam(sview name, sview type_name, Any def = {});

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
