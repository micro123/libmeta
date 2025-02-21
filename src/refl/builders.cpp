#include "refl/builders.hpp"
#include <format>
#include <memory>
#include <refl/field.hpp>
#include <refl/method.hpp>
#include <refl/registry.hpp>
#include <utility>
#include "refl/any.hpp"
#include "refl/constant.hpp"
#include "refl/property_container.hpp"

class Meta::MethodBuilder::PrivateData
{
public:
    bool      is_member_ {};
    MethodPtr method_;
};

class Meta::TypeBuilder::PrivateData
{
public:
    PropertyContainer *last_item{};
    MyTypePtr          type_;
    TypeId             type_id_;
};

Meta::MethodBuilder &Meta::MethodBuilder::AddParam (u32 idx, sview name, Any def)
{
    d->method_->AddParamInfo (idx + (d->is_member_ ? 1 : 0), name, std::move (def));
    return *this;
}

Meta::MethodPtr Meta::MethodBuilder::Build () const
{
    if (!d->method_->Verify ())
        throw std::runtime_error ("Method Verify Failed!");
    return std::move (d->method_);
}

Meta::MethodBuilder::~MethodBuilder ()
{
    delete d;
}

Meta::TypeBuilder &Meta::TypeBuilder::AddBaseType (TypeId id, Type::CastProc cast)
{
    d->type_->AddBaseClass (id, cast);
    return *this;
}

Meta::TypeBuilder &Meta::TypeBuilder::AddField (FieldPtr field)
{
    assert (field);
    d->last_item = field.Get();
    d->type_->AddField (std::move (field));
    return *this;
}

Meta::TypeBuilder &Meta::TypeBuilder::AddConstant (ConstantPtr constant)
{
    assert (constant);
    d->last_item = constant.Get();
    d->type_->AddConstant (std::move (constant));
    return *this;
}

Meta::TypeBuilder &Meta::TypeBuilder::AddMethod (MethodPtr method)
{
    assert (method);
    d->last_item = method.Get();
    d->type_->AddMethod (std::move (method));
    return *this;
}
Meta::TypeBuilder &Meta::TypeBuilder::AddConstructor (MethodPtr method)
{
    assert (method);
    d->last_item = method.Get();
    d->type_->AddConstructor (std::move (method));
    return *this;
}
Meta::TypeBuilder &Meta::TypeBuilder::AddCastTo (Type::CastProc proc, const TypeId &type_id)
{
    d->type_->AddConversion (proc, type_id);
    return *this;
}
Meta::TypeBuilder &Meta::TypeBuilder::AddConvertFrom (Type::ConvertProc proc, const TypeId &type_id)
{
    d->type_->AddConverter (proc, type_id);
    return *this;
}

Meta::TypeBuilder &Meta::TypeBuilder::WithProperties (const std::initializer_list<PropertyItem> &properties)
{
    if (d->last_item && properties.size())
    {
        for (auto &x: properties) {
            d->last_item->AppendProperty(x.name, x.value);
        }
    }
    return *this;
}

void Meta::TypeBuilder::Register () const
{
    TypePtr type = d->type_;
    Registry::Instance ().RegisterType (type, d->type_id_);
}

Meta::TypeBuilder::~TypeBuilder ()
{
    delete d;
}

Meta::MethodBuilder::MethodBuilder (MethodPtr method) : d (new PrivateData)
{
    d->method_    = std::move (method);
    d->is_member_ = d->method_->IsMember ();
}
Meta::Any Meta::TypeBuilder::EnumToString (const Any &value)
{
    auto type = value.Type ();
    assert (type);
    auto const constants = type->GetConstants ();
    auto       v1        = value.Value<u64> ();
    for (auto &x: constants)
    {
        if (x->Value () == v1)
        {
            return std::format ("{}::{}", type->Name (), x->Name ());
        }
    }
    return std::format ("{}::<Unknown>(0x{:X})", type->Name (), v1);
}
bool Meta::TypeBuilder::EnumFromString (const Any &obj, const Any &value)
{
    auto type = obj.Type ();
    assert (type);
    if (Any enum_v; AnyCast (value, enum_v, GetTypeId<s64> ()))
    {
        return type->ConvertFrom (enum_v, obj, enum_v.Id ());
    }
    auto       key       = value.Value<str> ();
    auto const constants = type->GetConstants ();
    for (auto &x: constants)
    {
        if (x->Name () == key)
        {
            return type->ConvertFrom (x->Value (), obj, GetTypeId<s64> ());
        }
    }
    return false;
}

Meta::TypeBuilder::TypeBuilder (MyTypePtr type, const TypeId &tid) : d (new PrivateData)
{
    d->type_    = std::move (type);
    d->type_id_ = tid;
    d->last_item = d->type_.Get();
    Register ();
}
