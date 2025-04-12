#ifndef LIBMETA_FIELD_INL
#define LIBMETA_FIELD_INL

#include <cstring>
#include "builders.hpp"

namespace Meta
{
    namespace details
    {
        // normal pointer
        template <typename T>
        class NormalField : public Field
        {
        public:
            NormalField (sview name, T *ptr) : Field (name), ptr_ (ptr)
            {
                assert (ptr_ != nullptr);
            }
            ~NormalField () {}

            bool IsMember () const override
            {
                return false;
            }
            bool IsConst () const override
            {
                return std::is_const_v<T>;
            }
            TypePtr Type () const override
            {
                return TypeOf<T> ();
            }

            Any Get (const Any *, u32 index) const override
            {
                assert (index == 0);
                return Any::RefWrap (ptr_, 1);
            }
            Any Set (const Any *, Any value, u32 index) const override
            {
                assert (index == 0);
                if constexpr (std::is_const_v<T>)
                {
                    return Get (nullptr, index);
                }
                else
                {
                    *ptr_ = value.ValueRef<T> ();
                    return Get (nullptr, index);
                }
            }

        private:
            T *ptr_;
        };

#ifdef EXPLICIT_CLASS_NAME
        template <typename C, typename T>
#else
        template <typename T>
#endif
        class MemberField : public Field
        {
            using Ptr = T (M_T::*);

        public:
#ifndef EXPLICIT_CLASS_NAME
            template <typename C>
#endif
            MemberField (sview name, T (C::*ptr)) : Field (name), ptr_ ((Ptr) ptr), cls_id_ (GetTypeId<C> ())
            {
            }
            ~MemberField () override {}

            bool IsMember () const override
            {
                return true;
            }
            bool IsConst () const override
            {
                return std::is_const_v<T>;
            }
            TypePtr Type () const override
            {
                return TypeOf<T> ();
            }

            Any Get (const Any *object, u32 index) const override
            {
                assert (index == 0);
                auto t_obj = GetObject (object);
                return Any::RefWrap (&(t_obj->*ptr_), 1);
            }
            Any Set (const Any *object, Any value, u32 index) const override
            {
                assert (index == 0);
                auto t_obj = GetObject (object);
                if constexpr (std::is_const_v<T>)
                {
                    return Get (object, index);
                }
                else
                {
                    t_obj->*ptr_ = value.ValueRef<T> ();
                    return Get (object, index);
                }
            }

        private:
            inline M_T *GetObject (const Any *object) const
            {
                if (object->Id () == cls_id_)
                    return (M_T *) object->ValuePtr<void> ();
                else
                {
                    // try cast
                    Any cls_obj;
                    if (AnyCast (*object, cls_obj, cls_id_))
                        return (M_T *) cls_obj.ValuePtr<void> ();
                }
                return nullptr;
            }

            Ptr          ptr_;
            TypeId const cls_id_;
        };

        template <typename T>
        class NormalArrayField : public Field
        {
            using Ptr = T (*)[];

        public:
            template <size_t N>
            NormalArrayField (sview name, T (*ptr)[N]) : Field (name), ptr_ ((Ptr) ptr), cnt_ (N)
            {
            }
            ~NormalArrayField () = default;

            bool IsMember () const override
            {
                return false;
            }
            bool IsConst () const override
            {
                return std::is_const_v<T>;
            }
            bool IsArray () const override
            {
                return true;
            }
            u32 Count () const override
            {
                return (u32) cnt_;
            }
            TypePtr Type () const override
            {
                return TypeOf<T> ();
            }

            Any Get (const Any *, u32 index) const override
            {
                assert (index < cnt_);
                T *ptr = (*ptr_) + index;
                return Any::RefWrap (ptr, cnt_ - index);
            }
            Any Set (const Any *, Any value, u32 index) const override
            {
                assert (index < cnt_);
                if constexpr (std::is_const_v<T> || std::is_array_v<T>)
                {
                    return Get (nullptr, index);
                }
                else
                {
                    (*ptr_)[index] = value.ValueRef<T> ();
                    return Get (nullptr, index);
                }
            }

        private:
            Ptr          ptr_;
            size_t const cnt_;
        };

#ifdef EXPLICIT_CLASS_NAME
        template <typename C, typename T>
#else
        template <typename T>
#endif
        class MemberArrayField : public Field
        {
            using Ptr = T (M_T::*)[];

        public:
#ifndef EXPLICIT_CLASS_NAME
            template <typename C, size_t N>
#else
            template <size_t N>
#endif
            MemberArrayField (sview name, T (C::*ptr)[N]) : Field (name), ptr_ ((Ptr) ptr), cnt_ (N), cls_id_ (GetTypeId<C> ())
            {
            }
            ~MemberArrayField () = default;

            bool IsMember () const override
            {
                return true;
            }
            bool IsConst () const override
            {
                return std::is_const_v<T>;
            }
            bool IsArray () const override
            {
                return true;
            }
            u32 Count () const override
            {
                return (u32) cnt_;
            }
            TypePtr Type () const override
            {
                return TypeOf<T> ();
            }

            Any Get (const Any *object, u32 index) const override
            {
                assert (index < cnt_);
                auto t_obj = GetObject (object);
                T   *ptr   = (t_obj->*ptr_) + index;
                return Any::RefWrap (ptr, cnt_ - index);
            }
            Any Set (const Any *object, Any value, u32 index) const override
            {
                assert (index < cnt_);
                auto t_obj = GetObject (object);
                if constexpr (std::is_const_v<T> || std::is_array_v<T>)
                {
                    return Get (object, index);
                }
                else
                {
                    (t_obj->*ptr_)[index] = value.ValueRef<T> ();
                    return Get (object, index);
                }
            }

        private:
            inline M_T *GetObject (const Any *object) const
            {
                if (object->Id () == cls_id_)
                    return (M_T *) object->ValuePtr<void> ();
                else
                {
                    // try cast
                    Any cls_obj;
                    if (AnyCast (*object, cls_obj, cls_id_))
                        return (M_T *) cls_obj.ValuePtr<void> ();
                }
                return nullptr;
            }

            Ptr          ptr_;
            size_t const cnt_;
            TypeId const cls_id_;
        };

        template <std::integral T>
        struct BitAccessor {
            u32 bytes_skip : 26;  // up to 64MB, it will be ok
            u32 shift_count : 6;  // up to 63
            u32 count;

            class View : public IView {
            public:
                View (const BitAccessor *accessor, void *base) : accessor_ (accessor), base_ (base) {}
                ~View () override {}

                Any Get (TypeId request) const override
                {
                    Any in = accessor_->_Read (base_);
                    Any out;
                    if (AnyCast (in, out, request))
                        return out;
                    return {};
                }
                bool Set (Any source) const override
                {
                    accessor_->Write (base_, source);
                    return true;
                }
            private:
                const BitAccessor *accessor_;
                void              *base_;
            };

            BitAccessor (u32 offset, u32 count)
            {
                bytes_skip  = offset / 8;
                shift_count = offset % 8;
                this->count = count;
            }

            Any Read (void *base) const
            {
                return MakeRef(static_cast<IView*>(new View(this, base)));
            }

            void Write (void *base, Any any) const
            {
                auto      *p          = static_cast<s8 *> (base);
                T         &memory     = *reinterpret_cast<T *> (&p[bytes_skip]);
                auto const value_mask = ((T {1} << count) - 1) << shift_count;

                // clear bits
                memory &= ~value_mask;

                T to_be_store = any.Value<T> ();
                if constexpr (std::is_signed_v<T>)
                {
                    _Fold (to_be_store, count);
                }

                // set bits
                memory |= (to_be_store << shift_count);
            }

        private:
            T _Read(const void *base) const {
                auto      *p = static_cast<const s8 *> (base);
                T          ret;
                auto const value_mask = (T {1} << count) - 1;
                ret                   = (*reinterpret_cast<const T *> (&p[bytes_skip]) >> shift_count) & value_mask;
                if constexpr (std::is_signed_v<T>)
                {
                    _Expand (ret, count);
                }
                return ret;
            }    

            static void _Expand (T &v, u32 bits)
            {
                const T bit = v & (T (1) << (bits - 1));
                if (bit)
                {
                    // negative value
                    // clear origin bit
                    v &= ~bit;
                    // set all high bits
                    v |= (T (-1) & ~(bit - 1));
                }
            }
            static void _Fold (T &v, u32 bits)
            {
                // cut redundant bits
                v &= ((T (1) << bits) - 1);
            }
        };

        template <typename T>
        class MemberBitField final : public Field
        {
            using Accessor = BitAccessor<std::remove_cv_t<T>>;

        public:
            template <typename C>
            MemberBitField (sview name, C *, size_t begin, size_t length) : Field (name), accessor_ (begin, length), cls_id_ (GetTypeId<C> ())
            {
            }
            ~MemberBitField () override {}

            bool IsMember () const override
            {
                return true;
            }
            bool IsConst () const override
            {
                return std::is_const_v<T>;
            }
            TypePtr Type () const override
            {
                return TypeOf<T> ();
            }

            Any Get (const Any *object, u32 index) const override
            {
                assert (index == 0);
                auto t_obj = GetObject (object);
                if (!t_obj)
                    return {};
                return accessor_.Read (t_obj);
            }
            Any Set (const Any *object, Any value, u32 index) const override
            {
                assert (index == 0);
                auto t_obj = GetObject (object);
                if (!t_obj)
                    return {};
                if constexpr (std::is_const_v<T>)
                {
                    return Get (object, index);
                }
                else
                {
                    accessor_.Write (t_obj, value);
                    return Get (object, index);
                }
            }

        private:
            inline void *GetObject (const Any *object) const
            {
                if (object->Id () == cls_id_)
                    return object->ValuePtr<void> ();
                else
                {
                    // try cast
                    Any cls_obj;
                    if (AnyCast (*object, cls_obj, cls_id_))
                        return cls_obj.ValuePtr<void> ();
                }
                return nullptr;
            }

        private:
            Accessor accessor_;
            TypeId   cls_id_;
        };

        template <typename T>
        FieldPtr MakeField (sview name, T *ptr)
        {
            return new NormalField {name, ptr};
        }

        template <typename C, typename T>
        FieldPtr MakeField (sview name, T (C::*ptr))
        {
#ifdef EXPLICIT_CLASS_NAME
            return new MemberField<C, T> {name, ptr};
#else
            return new MemberField<T> {name, ptr};
#endif
        }

        template <typename T, size_t N>
        FieldPtr MakeField (sview name, T (*ptr)[N])
        {
            return new NormalArrayField {name, ptr};
        }

        template <typename T, typename C, size_t N>
        FieldPtr MakeField (sview name, T (C::*ptr)[N])
        {
#ifdef EXPLICIT_CLASS_NAME
            return new MemberArrayField<C, T> {name, ptr};
#else
            return new MemberArrayField<T> {name, ptr};
#endif
        }
    }  // namespace details

    template <typename T>
    FieldPtr MakeField (sview name, T t)
    {
        return details::MakeField (name, t);
    }

    template <typename C, typename T>
    FieldPtr BitField (sview name, size_t begin, size_t count)
    {
        assert (sizeof (C) * 8 /* total bits */ >= begin + count);
        assert (sizeof (T) * 8 >= count);  // type limits
        return new details::MemberBitField<T> (name, (C *) (uintptr_t) 0x1, begin, count);
    }
}  // namespace Meta

#endif /* LIBMETA_FIELD_INL */
