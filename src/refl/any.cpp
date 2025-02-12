#include "refl/any.hpp"
#include "any_converter.hpp"
#include <unordered_map>
#include "refl/method.hpp"

using cvt_key_t = std::pair<Meta::TypeId, Meta::TypeId>;

template <>
struct std::hash<cvt_key_t>
{
    size_t operator()(const cvt_key_t &e) const {
        return e.first.hash() << 16 | e.second.hash() >> 16;
    }
};

static std::unordered_map<cvt_key_t, Meta::Type::CastPorc> g_fundamental_cast_ops;

static void __register_fundamental_type_cast_ops() {
    auto& m = g_fundamental_cast_ops;
    using namespace Meta;
#define F(x,y) \
    m[std::make_pair(GetTypeId<x>(), GetTypeId<y>())] = &Converter<x,y>::Convert
#define REG_CAST_OP_1(dst) \
    F(s8, dst); \
    F(u8, dst); \
    F(s16, dst); \
    F(u16, dst); \
    F(s32, dst); \
    F(u32, dst); \
    F(s64, dst); \
    F(u64, dst); \
    F(f32, dst); \
    F(f64, dst); \
    F(bool, dst); \
    F(char, dst); \
    F(cstr, dst); \
    F(str, dst)

    FUNDAMENTAL_TYPES(REG_CAST_OP_1);
}

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
Meta::Any Meta::Any::operator() (Any *args, size_t cnt) const
{
    // 需要是函数才能调用
    if (auto m = ValuePtr<Method>()) {
        return m->InvokeWithArgs(args, cnt);
    }
    return {};
}
Meta::Any::Any (TypeId tid)
{
    type_id_ = tid;
}
void Meta::Any::Destroy () const
{
    if (data_ && ops_.Destroy)
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

bool Meta::AnyCast(const Any &in, TypeId src, Any& out, TypeId dst)
{
    // 1. check for fundamental types cvt
    static struct CastMapInit{
        CastMapInit() {
            __register_fundamental_type_cast_ops();
        }
    } __init__;
    auto key = std::make_pair(src, dst);
    if (g_fundamental_cast_ops.find(key) != end(g_fundamental_cast_ops))
    {
        out = g_fundamental_cast_ops.at(key)(in);
        return out.Valid();
    }

    // 2. use converter from type
    auto type = TypeOf(src);
    if (!type)
        return false;
    
    return type->CanCast(dst) && type->Cast(in, out, dst);
}

constexpr size_t any_size = sizeof(Meta::Any);
