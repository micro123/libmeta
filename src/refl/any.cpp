#include "refl/any.hpp"
#include <unordered_map>
#include "any_converter.hpp"
#include "refl/constant.hpp"
#include "refl/field.hpp"
#include "refl/method.hpp"

using cvt_key_t = std::pair<Meta::TypeId, Meta::TypeId>;

template <>
struct std::hash<cvt_key_t> {
    size_t operator() (const cvt_key_t &e) const
    {
        return e.first.hash () << 16 | e.second.hash () >> 16;
    }
};

static std::unordered_map<cvt_key_t, Meta::Type::CastPorc> g_fundamental_cast_ops;

static void __register_fundamental_type_cast_ops ()
{
    auto &m = g_fundamental_cast_ops;
    using namespace Meta;
#define F(x, y) m[std::make_pair (GetTypeId<x> (), GetTypeId<y> ())] = &Converter<x, y>::Convert
#define REG_CAST_OP_1(dst) \
    F (s8, dst);           \
    F (u8, dst);           \
    F (s16, dst);          \
    F (u16, dst);          \
    F (s32, dst);          \
    F (u32, dst);          \
    F (s64, dst);          \
    F (u64, dst);          \
    F (f32, dst);          \
    F (f64, dst);          \
    F (bool, dst);         \
    F (char, dst);         \
    F (cstr, dst);         \
    F (str, dst)

    FUNDAMENTAL_TYPES (REG_CAST_OP_1);
}

const Meta::Any Meta::Any::Void {Meta::GetTypeId<void> ()};

const Meta::details::AnyOps* Meta::details::AnyOps::Empty ()
{
    static AnyOps empty{};
    return &empty;
}
Meta::Any::Any ()
{
    ops_ = details::AnyOps::Empty ();
}
Meta::Any::Any (const Any &var) : Any ()
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
    Assign (std::move(var));
    return *this;
}
Meta::TypePtr Meta::Any::Type () const
{
    return TypeOf (type_id_);
}
Meta::Any Meta::Any::CallWithArgs (Meta::Any *args, size_t cnt) const
{
    // 1. this is a method
    if (auto m = ValuePtr<Method> ())
    {
        return m->InvokeWithArgs (args, cnt);
    }
    // 2. cnt >= 1 and args[0] is method name
    else if (cnt >= 1)
    {
        str  method_name = args[0].Value<str> ();
        auto type        = Type ();
        if (type)
        {
            if (auto m = type->GetMethod (method_name))
            {
                // replace args[0] with this
                args[0] = *this;
                return m->InvokeWithArgs (args, cnt);
            }
        }
    }
    return {};
}
Meta::Any::Any (TypeId tid)
{
    type_id_ = tid;
    ops_     = details::AnyOps::Empty ();
    cnt_     = 0;
}
Meta::Any::Any (void *data, const TypeId& tid, const details::AnyOps *ops, size_t cnt)
{
    ops_ = ops;
    type_id_ = tid;
    data_ = ops_->Clone (&buffer_, &data);
    cnt_ = cnt;
}
Meta::Any Meta::Any::operator[] (Meta::str key) const
{
    assert (IsValid ());

    auto const type = Type ();
    // must have type
    if (!type)
        return {};

    // 1. try field
    if (auto f = type->GetField (key))
    {
        return f->Get (this);
    }

    if (auto c = type->GetConstant (key))
    {
        return c->Value ();
    }

    return {};
}

Meta::Any Meta::Any::operator[] (size_t index) const {
    if (ops_->At == nullptr || index >= cnt_)
        return {}; // not support
    return {ops_->At(Get (), index), type_id_, ops_, cnt_ - index};
}

void Meta::Any::Destroy () const
{
    if (data_ && ops_->Destroy)
    {
        ops_->Destroy (data_);
    }
}
void Meta::Any::Assign (const Any &any)
{
    if (any.IsValid ())
    {
        type_id_ = any.type_id_;
        ops_     = any.ops_;
        if (any.data_)
            data_ = any.ops_->Clone (&buffer_, any.data_);
        cnt_ = 1;
    }
}
void Meta::Any::Assign (Any &&any)
{
    if (any.IsValid ())
    {
        type_id_ = any.type_id_;
        ops_     = any.ops_;
        if (any.IsSmallObj ())
        {
            data_ = any.ops_->Clone (&buffer_, any.data_);
        }
        else
        {
            std::swap (data_, any.data_);
        }
        cnt_ = any.cnt_;
    }
}

bool Meta::AnyCast (const Any &in, TypeId src, Any &out, TypeId dst)
{
    // 1. check for fundamental types cvt
    static struct CastMapInit {
        CastMapInit ()
        {
            __register_fundamental_type_cast_ops ();
        }
    } __init__;
    auto key = std::make_pair (src, dst);
    if (g_fundamental_cast_ops.find (key) != end (g_fundamental_cast_ops))
    {
        out = g_fundamental_cast_ops.at (key) (in);
        if (out.IsValid ())
            return true;
    }

    // 2. use converter from type
    auto type = TypeOf (src);
    if (!type)
        return false;

    return type->CanCast (dst) && type->Cast (in, out, dst);
}

std::ostream &operator<< (std::ostream &o, const Meta::Any &any)
{
    if (any.IsValid())
        o << any.Value<Meta::str> ();
    else
        o << "nil";
    return o;
}
