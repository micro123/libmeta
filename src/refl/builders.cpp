#include "refl/builders.hpp"
#include <format>
#include <memory>
#include <refl/field.hpp>
#include <refl/method.hpp>
#include <refl/registry.hpp>
#include <utility>
#include "refl/any.hpp"
#include "refl/constant.hpp"

class Meta::MethodBuilder::PrivateData
{
public:
    MethodPtr method_;
};

class Meta::TypeBuilder::PrivateData
{
public:
    MyTypePtr type_;
    TypeId    type_id_;
};

Meta::MethodBuilder &Meta::MethodBuilder::AddParam (sview name, TypeId type_id, Any def)
{
    d->method_->AddParam (name, type_id, std::move (def));
    return *this;
}

Meta::MethodPtr      Meta::MethodBuilder::Build () const
{
    return std::move(d->method_);
}

Meta::MethodBuilder::~MethodBuilder ()
{
    delete d;
}

Meta::TypeBuilder &Meta::TypeBuilder::AddField (FieldPtr field)
{
    assert (field);
    d->type_->AddField (std::move(field));
    return *this;
}

Meta::TypeBuilder &Meta::TypeBuilder::AddConstant (ConstantPtr constant)
{
    assert (constant);
    d->type_->AddConstant (std::move (constant));
    return *this;
}

Meta::TypeBuilder &Meta::TypeBuilder::AddMethod (MethodPtr method)
{
    assert (method);
    d->type_->AddMethod (std::move (method));
    return *this;
}
Meta::TypeBuilder &Meta::TypeBuilder::AddCastTo (Type::CastPorc proc, const TypeId &type_id)
{
    d->type_->AddConversion (proc, type_id);
    return *this;
}
Meta::TypeBuilder &Meta::TypeBuilder::AddConvertFrom (Type::ConvertProc proc, const TypeId &type_id)
{
    d->type_->AddConverter (proc, type_id);
    return *this;
}

Meta::TypePtr      Meta::TypeBuilder::Register(TypeId tid) const
{
    if (tid == NULL_TYPE_ID)
        tid = d->type_id_;
    assert (TypeOf(tid) == nullptr); // 必须没有注册过
    TypePtr type = d->type_;
    Registry::Instance ().RegisterType (type, tid);
    return std::move(type);
}

Meta::TypeBuilder::~TypeBuilder ()
{
    delete d;
}

Meta::MethodBuilder::MethodBuilder (MethodPtr method) : d (new PrivateData)
{
    d->method_ = std::move (method);
}
Meta::Any Meta::TypeBuilder::EnumToString (const Any &value)
{
    auto type = value.Type ();
    assert (type);
    auto const constants = type->GetConstants ();
    auto v1 = value.Value<u64> ();
    for (auto &x: constants)
    {
        if (x->Value () == v1)
        {
            return std::format("{}::{}", type->Name (), x->Name ());
        }
    }
    return std::format ("{}::<Unknown>({:x})", type->Name (), v1);
}
bool      Meta::TypeBuilder::EnumFromString (const Any &obj, const Any &value)
{
    auto type = value.Type ();
    assert (type);
    if (Any enum_v; AnyCast (value, value.Id (), enum_v, GetTypeId<s64> ()))
    {
        return type->ConvertFrom (enum_v, obj, enum_v.Id ());
    }
    auto key = value.Value<str> ();
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

Meta::TypeBuilder::TypeBuilder (MyTypePtr type, const TypeId& tid): d (new PrivateData)
{
    d->type_ = std::move (type);
    d->type_id_ = tid;
}
