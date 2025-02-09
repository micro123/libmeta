#include "refl/builders.hpp"
#include <memory>
#include <refl/field.hpp>
#include <refl/method.hpp>
#include <refl/registry.hpp>
#include <utility>
#include "refl/any.hpp"

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

Meta::MethodBuilder &Meta::MethodBuilder::AddParam (sview name, sview type_name, Any def)
{
    d->method_->AddParam (name, Registry::Instance ().Get (type_name.data ()), std::move (def));
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

Meta::MethodBuilder::MethodBuilder (MethodPtr method): d (new PrivateData)
{
    d->method_ = std::move(method);
}

Meta::TypeBuilder::TypeBuilder (MyTypePtr type, TypeId tid): d (new PrivateData)
{
    d->type_ = std::move (type);
    d->type_id_ = tid;
}
