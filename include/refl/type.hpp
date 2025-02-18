#ifndef TYPE_HPP
#define TYPE_HPP

#include <string_view>
#include <vector>
#include <unordered_map>
#include "fwd.hpp"
#include "utility/constexpr.hpp"

namespace Meta
{
    enum TypeFlags : u32
    {
        eTypeIsPod     = 0x1,
        eTypeIsConst   = 0x2,
        eTypeIsRef     = 0x4,
        eTypeIsPtr     = 0x8,
        eTypeIsVirtual = 0x10,
        eTypeIsEnum    = 0x20,
    };

    template <typename T>
    TypeId GetTypeId ();

    class LIBMETA_API Type
    {
    public:
        using CastPorc = Any(*)(const Any &in); // 内容转换函数
        using ConvertProc = bool(*)(const Any &out, const Any &in); // 从别的东西构造
        Type (sview name, size_t size, u32 flags);
        virtual ~Type ();

        Type (const Type &)            = delete;
        Type &operator= (const Type &) = delete;

        // 类型基本信息
        inline sview Name () const
        {
            return name_;
        }
        inline size_t Size () const
        {
            return size_;
        }

        // 类型特性
        inline bool IsPod () const
        {
            return flags_ & eTypeIsPod;
        }
        inline bool IsConst () const
        {
            return flags_ & eTypeIsConst;
        }
        inline bool IsRef () const
        {
            return flags_ & eTypeIsRef;
        }
        inline bool IsPtr () const
        {
            return flags_ & eTypeIsPtr;
        }
        inline bool IsVirtual () const
        {
            return flags_ & eTypeIsVirtual;
        }
        inline bool IsEnum () const
        {
            return flags_ & eTypeIsEnum;
        }

        // 成员信息
        virtual std::vector<FieldPtr> GetFields () const;
        virtual FieldPtr              GetField (sview name);

        // 函数信息
        virtual std::vector<MethodPtr> GetMethods () const;
        virtual MethodPtr              GetMethod (sview name) const;

        // 常量信息（枚举）
        virtual std::vector<ConstantPtr> GetConstants () const;
        virtual ConstantPtr              GetConstant (sview name) const;

        // 基类信息
        virtual std::vector<TypePtr>     GetBaseClasses () const;

        // 一般工具
        virtual str                      ToString (const Any &obj) const;
        virtual bool                     FromString (const Any &obj, const str& data) const;

        template <typename T>
        inline void AddConverter(ConvertProc proc) { return AddConverter (proc, GetTypeId<T> ()); }
        void AddConverter (ConvertProc proc, const TypeId& type_id);

        template <typename T>
        inline void AddConversion(CastPorc proc) { return AddConversion(proc, GetTypeId<T>()); }
        void AddConversion(CastPorc proc, const TypeId& type_id);

        // cast check
        template <typename T>
        bool CanCast() const { return CanCast(GetTypeId<T>()); }
        bool CanCast(const TypeId& dst) const;

        template <typename T>
        bool Cast(const Any &obj, Any &out) const { return Cast(obj, out, GetTypeId<T>()); }
        bool Cast(const Any &obj, Any &out, const TypeId& dst) const;

        template <typename T>
        bool CanConvertFrom() const { return CanConvertFrom(GetTypeId<T>()); }
        bool CanConvertFrom(const TypeId& src) const;

        template <typename T>
        bool ConvertFrom(T &&in, const Any &out) const { return ConvertFrom(in, out, GetTypeId<T>()); }
        bool ConvertFrom(cstr in, const Any &out) const;
        bool ConvertFrom(const Any &in, const Any &out, const TypeId& src) const;
    private:
        sview const  name_;
        size_t const size_;
        u32 const    flags_;
    protected:
        std::unordered_map<TypeId, CastPorc>      cast_ops_;
        std::unordered_map<TypeId, ConvertProc>   construct_ops_;
    };

    namespace details {
        template <typename T>
        TypeId GetTypeIdImpl ();

        // template <typename T, size_t N>
        // TypeId GetTypeId<T(&)[N]> () {
        //     return GetTypeId<T>();
        // }

        // template <typename T>
        // TypeId GetTypeId<T*> () {
        //     return GetTypeId<T>();
        // }

        template <typename T>
        TypeId GetTypeIdImpl ()
        {
            static StringName store(GetTypeName<T>());
            return store;
        }
    }

    template <typename T>
    TypeId GetTypeId ()
    {
        using no_pointer_t = std::remove_pointer_t<T>;
        using no_cvref_t = std::remove_cvref_t<no_pointer_t>;
        return details::GetTypeIdImpl<no_cvref_t>();
    }

    LIBMETA_API
    TypePtr TypeOf (const TypeId& id);

    template <typename T>
    TypePtr TypeOf ()
    {
        return TypeOf (GetTypeId<T> ());
    }

    inline TypePtr NilType() { return TypeOf(NULL_TYPE_ID); }

    template <typename T>
    constexpr u32 CalcTypeFlags ();
}  // namespace Meta

#include "type.inl"

#endif /* TYPE_HPP */
