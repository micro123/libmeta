#ifndef TYPE_HPP
#define TYPE_HPP

#include <string_view>
#include <vector>
#include "fwd.hpp"

namespace Meta
{
    enum TypeFlags : u32
    {
        eTypeIsPod     = 0x1,
        eTypeIsConst   = 0x2,
        eTypeIsRef     = 0x4,
        eTypeIsPtr     = 0x8,
        eTypeIsVirtual = 0x10,
    };

    class LIBMETA_API Type
    {
    public:
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
        virtual std::string              ToString (const Any &obj) const;

    private:
        sview const  name_;
        size_t const size_;
        u32 const    flags_;
    };

    namespace details {
        template <typename T>
        TypeId GetTypeId ();

        // template <typename T, size_t N>
        // TypeId GetTypeId<T(&)[N]> () {
        //     return GetTypeId<T>();
        // }

        // template <typename T>
        // TypeId GetTypeId<T*> () {
        //     return GetTypeId<T>();
        // }

        template <typename T>
        TypeId GetTypeId ()
        {
            static u8 store;
            return reinterpret_cast<TypeId> (&store);
        }
    }

    template <typename T>
    TypeId GetTypeId ()
    {
        using no_pointer_t = std::remove_pointer_t<T>;
        using no_cvref_t = std::remove_cvref_t<no_pointer_t>;
        return details::GetTypeId<no_cvref_t>();
    }

    LIBMETA_API
    TypePtr TypeOf (TypeId id);

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
