#ifndef LIBMETA_STRING_NAME_HPP
#define LIBMETA_STRING_NAME_HPP

#include "exports.h"

#include <atomic>
#include <string>

namespace Meta {
    struct LIBMETA_API StaticCString {
        const char *str;
        static StaticCString create(const char*pstr);
        static StaticCString create(std::string_view sv);
    };

    class LIBMETA_API StringName final {
        enum {
            TABLE_BITS = 16,
            TABLE_SIZE = 1 << TABLE_BITS,
            TABLE_MASK = TABLE_SIZE - 1,
        };

        struct Data {
            std::atomic<u32> ref{0};
            const char *cstr{};
            std::string str;
            s32         idx{};
            u32         hash{};
            Data       *prev{};
            Data       *next{};

            inline std::string GetName() const { return cstr ? std::string{cstr} : str; };
        };

        Data *data_{};

        static Data *s_table_[TABLE_SIZE];

        void unref();
        StringName(Data *data): data_(data) {}
    public:
        StringName() = default;
        explicit StringName(const std::string_view &sv);
        explicit StringName(const char *cstr, u32 len = 0);
        explicit StringName(const std::string &str);
        StringName(const StringName &other);
        StringName(const StaticCString &cstr);
        ~StringName();

        StringName &operator=(const StringName &other);

        inline operator const void* () const {
            return data_ && (data_->cstr || !data_->str.empty()) ? (void*)1 : nullptr;
        }

        inline std::strong_ordering operator<=>(const StringName &other) const {
            return data_ <=> other.data_;
        }

        inline u32 hash() const {
            return data_ ? data_->hash : 0;
        }

        inline operator std::string() const {
            if (data_) {
                return data_->cstr ? std::string{data_->cstr} : data_->str;
            }
            return {};
        }
    };

    bool LIBMETA_API operator==(const std::string &p_name, const StringName &p_string_name);
    bool LIBMETA_API operator!=(const std::string &p_name, const StringName &p_string_name);
    bool LIBMETA_API operator==(const char *p_name, const StringName &p_string_name);
    bool LIBMETA_API operator!=(const char *p_name, const StringName &p_string_name);
}

#define SNAME(x) ([]{ return Meta::StringName{Meta::StaticCString::create(x)}; })()

template <>
struct std::hash<Meta::StringName> {
    size_t operator()(const Meta::StringName &tid) const { return tid.hash(); }
};

#endif /* LIBMETA_STRING_NAME_HPP */
