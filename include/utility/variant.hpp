#ifndef LIBMETA_VARIANT_HPP
#define LIBMETA_VARIANT_HPP

#include "exports.h"
#include "refl/type.hpp"
#include "utility/ref.hpp"

/**
 * 期望 Variant 可以:
 * 1. 从任意类型 T 构造
 * 2. 从任意类型 T 赋值
 * 3. 有值时可以转换为 T& / T*
 * 4. 当类型 T&/T* 可以赋值给类型 U&/U* 时，值也可以转换为 U&/U*
 * 5. 可以获取当前存储值的类型 TypeRef
 * 6. 简单的算数类型值之间可以转换，也可以转换成字符串 std::string, 但不能由 std::string 转换成数字
 * 7. 根据存储类型区分复制操作
 * 8. 可判空，与nullptr比较，提供Valid函数
 */

namespace Meta
{
    class LIBMETA_API Variant
    {
    public:
        // MARK: Constructors
        // Empty
        Variant () {}
        
        // Ref
        template<typename T>
        Variant (T &value): value_type_(TypeOf<T>()) {}

        // Copy / ConstRef / Construct
        template<typename T>
        Variant (const T &value): value_type_(TypeOf<T>()) {}

        // Construct
        template<typename T>
        Variant (T &&value): value_type_(TypeOf<T>()) {}

        // Ref<T>
        template<typename T>
        Variant(Ref<T> ref) {}

        Variant (const Variant& var) {}
        Variant (Variant &&var) {}

        // MARK: Assignment
        template <typename T>
        Variant &operator=(T &t) { // New Ref
            return *this;
        }
        template <typename T>
        Variant &operator=(T &&t) { // Move Value
            return *this;
        }
        template <typename T>
        Variant &operator=(const T &t) { // Const Ref
            return *this;
        }
        template <typename T>
        Variant &operator=(Ref<T> ref) { // Const Ref
            return *this;
        }
        Variant &operator=(const Variant &var) {
            return *this;
        }
        Variant &operator=(Variant &&var) {
            return *this;
        }

        template <typename T>
        T *ValuePtr() const {

        }

        template <typename T>
        T &ValueRef() const {

        }

        bool Valid() const {
            return false;
        }

        operator bool() const {
            return Valid();
        }

        bool operator==(decltype(nullptr)) const {
            return !Valid();
        }

        bool operator!=(decltype(nullptr)) const {
            return Valid();
        }

        inline TypePtr Type() const { return value_type_; }

    private:
        TypePtr      value_type_ {};
        
    };
}  // namespace Meta

#endif /* LIBMETA_VARIANT_HPP */
