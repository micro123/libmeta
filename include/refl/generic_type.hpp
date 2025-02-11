#ifndef LIBMETA_GENERIC_TYPE_HPP
#define LIBMETA_GENERIC_TYPE_HPP

#include <list>
#include "exports.h"
#include "refl/fwd.hpp"
#include "refl/type.hpp"

namespace Meta
{
    class LIBMETA_API GenericType : public Type
    {
        using FieldContainer     = std::list<std::pair<str, FieldPtr>>;
        using MethodContainer    = std::list<std::pair<str, MethodPtr>>;
        using ConstantsContainer = std::list<std::pair<str, ConstantPtr>>;
    public:
        using Ptr = Ref<GenericType>;
        using Type::Type;

        void AddField(FieldPtr ptr);
        void AddMethod(MethodPtr ptr);
        void AddConstant(ConstantPtr ptr);
        void AddBaseClass(TypePtr ptr);

        [[nodiscard]] std::vector<FieldPtr>    GetFields () const override;
        [[nodiscard]] FieldPtr                 GetField (sview name) override;
        [[nodiscard]] std::vector<MethodPtr>   GetMethods () const override;
        [[nodiscard]] MethodPtr                GetMethod (sview name) const override;
        [[nodiscard]] std::vector<ConstantPtr> GetConstants () const override;
        [[nodiscard]] ConstantPtr              GetConstant (sview name) const override;
    private:
        FieldContainer     fields_;
        MethodContainer    methods_;
        ConstantsContainer constants_;
    };
}

#endif //LIBMETA_GENERIC_TYPE_HPP
