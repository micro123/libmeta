#ifndef LIBMETA_TYPE_HPP
#define LIBMETA_TYPE_HPP

#include "fwd.hpp"

#include <string_view>

namespace Meta {
    using sview = std::string_view;

    enum TypeFlags: u32 {
        eTypeIsPod = 0x1,
        eTypeIsConst = 0x2,
        eTypeIsRef = 0x4,
        eTypeIsPtr = 0x8,
        eTypeIsVirtual = 0x10,
    };

    class LIBMETA_API Type {
    public:
        Type(sview name, size_t size, u32 flags);
        virtual ~Type();

        Type(const Type &) = delete;
        Type &operator=(const Type &) = delete;

        inline sview  Name() const { return name_; }
        inline size_t Size() const { return size_; }
        inline bool   IsPod() const { return flags_ & eTypeIsPod; }
        inline bool   IsConst() const { return flags_ & eTypeIsConst; }
        inline bool   IsRef() const { return flags_ & eTypeIsRef; }
        inline bool   IsPtr() const { return flags_ & eTypeIsPtr; }
        inline bool   IsVirtual() const { return flags_ & eTypeIsVirtual; }

    private:
        sview  const name_;
        size_t const size_;
        u32    const flags_;
    };

    template<typename T>
    TypeId GetTypeId() {
        static u8 store;
        return reinterpret_cast<TypeId>(&store);
    }

    LIBMETA_API
    Type *TypeOf(TypeId id);

    template <typename T>
    Type *TypeOf()
    {
        return TypeOf(GetTypeId<T>());
    }

    template <typename T>
    constexpr u32 CalcTypeFlags();
}

#include "type.inl"

#endif /* LIBMETA_TYPE_HPP */
