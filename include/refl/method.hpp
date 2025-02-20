#ifndef LIBMETA_METHOD_HPP
#define LIBMETA_METHOD_HPP

#include <vector>
#include "exports.h"
#include "refl/any.hpp"
#include "refl/fwd.hpp"
#include "refl/method.hpp"
namespace Meta
{
    struct ParameterInfo {
        sview  name;
        TypeId type_id;
        Any    def;
    };

    using Params = std::vector<ParameterInfo>;

    class LIBMETA_API Method
    {
        friend class MethodBuilder;

    public:
        Method (sview name, TypeId rtype, u32 param_count);
        virtual ~Method ();

        [[nodiscard]] inline sview Name () const
        {
            return name_;
        }
        [[nodiscard]] inline sview Description () const
        {
            return desc_;
        }

        TypeId         ResultType () const;
        virtual u32    ParameterCount () const;
        virtual TypeId ParameterType (u32 index) const;
        virtual sview  ParameterName (u32 index) const;
        virtual Any    ParameterDefault (u32 index) const;
        virtual s32    ParameterMatches (const Any *argv, const TypePtr *argt, u32 argc) const;

        virtual bool IsMember () const   = 0;
        virtual bool IsConst () const    = 0;
        virtual bool IsVolatile () const = 0;

        virtual Any InvokeWithArgs (Any *args, u32 cnt) const = 0;

        template <typename... Args>
        Any Invoke (Args... args) const
        {
            Any va_arr[] = {args..., {}};
            return InvokeWithArgs (va_arr, (u32) std::size (va_arr) - 1);
        }

        [[nodiscard]] str ToString () const;

        inline u32 DefaultParameterStart () const
        {
            return default_param_start_;
        }
        inline bool HasDefaultParameter () const
        {
            return ~default_param_start_ != 0;
        }
        inline bool ParameterCountCheck (u32 pass) const
        {
            u32 const total = ParameterCount ();
            return (pass == total) || (HasDefaultParameter () && pass + 1 >= DefaultParameterStart () && pass < total);
        }

    protected:
        void       AddParamInfo (u32 idx, sview name, Any def = {});
        void       SetArgType (u32 idx, TypeId id);
        void       SetArgTypes (TypeId *ids, u32 cnt);
        bool       Verify () const;
        const Any *ParameterOfDefault (Any *argv, u32 argn, u32 param_idx) const;

        template <typename... Args>
        inline void SetArgTypes ()
        {
            constexpr size_t argn = sizeof...(Args);
            if constexpr (argn > 0)
            {
                using TupleType = std::tuple<Args...>;
                SetArgTypes<TupleType> (std::make_index_sequence<argn> {});
            }
        }

        template <typename T, size_t... Idx>
        inline void SetArgTypes (std::index_sequence<Idx...>)
        {
            TypeId ids[] = {GetTypeId<std::tuple_element_t<Idx, T>> ()..., {}};
            SetArgTypes (ids, (u32) std::tuple_size_v<T>);
        }

    private:
        sview  name_;
        sview  desc_;
        TypeId result_type_;
        Params params_;
        u32    default_param_start_;
    };
}  // namespace Meta

#include "method.inl"

#endif /* LIBMETA_METHOD_HPP */
