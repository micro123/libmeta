#include "refl/any.hpp"

const Meta::Any Meta::Any::Void {Meta::GetTypeId<void> ()};

Meta::details::AnyOps Meta::details::AnyOps::Empty ()
{
    return {};
}
Meta::Any::Any ()
{
    ops_ = details::AnyOps::Empty ();
}
Meta::Any::Any (const Any &var): Any()
{
    Assign (var);
}
Meta::Any::Any (Any &&var) noexcept
{
    Assign (var);
}
Meta::Any::~Any ()
{
    Destroy ();
}
Meta::Any &Meta::Any::operator= (const Any &var)
{
    Destroy ();
    Assign (var);
    return *this;
}
Meta::Any &Meta::Any::operator= (Any &&var) noexcept
{
    Destroy ();
    Assign (var);
    return *this;
}
Meta::TypePtr Meta::Any::Type () const
{
    return TypeOf (type_id_);
}
Meta::Any::Any (TypeId tid)
{
    type_id_ = tid;
}
void Meta::Any::Destroy () const
{
    if (data_)
    {
        ops_.Destroy (data_);
    }
}
void Meta::Any::Assign (const Any &any)
{
    if (any.Valid ())
    {
        type_id_ = any.type_id_;
        ops_     = any.ops_;
        if (any.data_)
            data_ = any.ops_.Clone (&buffer_, any.data_);
    }
}
void Meta::Any::Assign (Any &&any)
{
    if (any.Valid ())
    {
        type_id_ = any.type_id_;
        ops_     = any.ops_;
        if (any.IsSmallObj ())
        {
            data_ = any.ops_.Clone (&buffer_, any.data_);
        }
        else
        {
            std::swap (data_, any.data_);
        }
    }
}