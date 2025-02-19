#ifndef LIBMETA_DELEGATE_HPP
#define LIBMETA_DELEGATE_HPP

#include "refl/fwd.hpp"
#include "refl/method.hpp"

namespace Meta
{
    class LIBMETA_API Delegate : public Method
    {
    public:
        template <typename... Args>
        Delegate (MethodPtr m, Args &&...args) : Delegate (m)
        {
            AppendArg (std::forward<Args> (args)...);
        }
        Delegate (MethodPtr m);
        ~Delegate () override;

        bool    IsMember () const override;
        bool    IsConst () const override;
        bool    IsVolatile () const override;
        u32     ParameterCount () const override;
        TypePtr ParameterType (u32 index) const override;
        sview   ParameterName (u32 index) const override;
        Any     ParameterDefault (u32 index) const override;

        Any InvokeWithArgs (Any *args, u32 cnt) const override;

        [[nodiscard]] str ToString () const;

    private:
        template <typename... Other>
        void AppendArg (const Any &any, Other &&...other)
        {
            AppendArg (any);
            AppendArg (std::forward<Other> (other)...);
        }
        void             AppendArg (const Any &any);
        MethodPtr        mehtod_;
        std::vector<Any> prefilled_args_;
    };
}  // namespace Meta

#endif /* LIBMETA_DELEGATE_HPP */
