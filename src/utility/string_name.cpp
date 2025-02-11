#include "utility/string_name.hpp"

#include <cstring>
#include <mutex>

namespace Meta
{
    u32 string_hash(const char *cstr, u32 len)
    {
        // static_cast: avoid negative values on platforms where char is signed.
        uint32_t hashv = 5381;
        uint32_t c = static_cast<uint8_t>(*cstr++);

        while (c) {
            hashv = ((hashv << 5) + hashv) + c; /* hash * 33 + c */
            c = static_cast<uint8_t>(*cstr++);
        }

        return hashv;
    }

    u32 string_hash(const std::string &str)
    {
        return string_hash(str.c_str(), str.length());
    }


    static std::mutex string_pool_mtx;
    using LockType = std::lock_guard<decltype(string_pool_mtx)>;
#define LOCK LockType lck(string_pool_mtx)

    StringName::Data* StringName::s_table_[StringName::TABLE_SIZE];

    StringName::StringName (const char *cstr, u32 len) {
        if (len == 0 && (len = strlen(cstr)) == 0)
            return; // empty string, ignored

        u32 hash = string_hash(cstr, len);
        u32 idx = hash & TABLE_MASK;

        LOCK;

        data_ = s_table_[idx];

        while(data_) {
            if (data_->hash == hash && data_->GetName() == cstr)
                break;
            data_ = data_->next;
        }

        if (data_) {
            data_->ref.fetch_add(1);
            return;
        }

        data_ = new Data;
        data_->ref.store(1);
        data_->str = std::string{cstr, cstr+len};
        data_->cstr = nullptr;
        data_->hash = hash;
        data_->idx = idx;
        data_->next = s_table_[idx];
        data_->prev = nullptr;

        if (s_table_[idx]) {
            s_table_[idx]->prev = data_;
        }

        s_table_[idx] = data_;
    }
    StringName::StringName (const std::string_view &sv): StringName(sv.data(), sv.size()) {

    }

    StringName::StringName (const std::string &str)
    {
        if (str.empty())
            return;

        LOCK;

        u32 hash = string_hash(str);
        u32 idx = hash & TABLE_MASK;

        data_ = s_table_[idx];

        while(data_) {
            if (data_->hash == hash && data_->GetName() == str)
                break;
            data_ = data_->next;
        }

        if (data_) {
            data_->ref.fetch_add(1);
            return;
        }

        data_ = new Data;
        data_->ref.store(1);
        data_->str = str;
        data_->cstr = nullptr;
        data_->hash = hash;
        data_->idx = idx;
        data_->next = s_table_[idx];
        data_->prev = nullptr;

        if (s_table_[idx]) {
            s_table_[idx]->prev = data_;
        }

        s_table_[idx] = data_;
    }
    StringName::StringName (const StringName &other) {
        if (other.data_) {
            other.data_->ref.fetch_add(1);
            data_ = other.data_;
        }
    }
    StringName::StringName (const StaticCString &cstr) {
        if (strlen(cstr.str) == 0)
            return;

        u32 hash = string_hash(cstr.str);
        u32 idx = hash & TABLE_MASK;
        
        LOCK;

        data_ = s_table_[idx];

        while(data_) {
            if (data_->hash == hash && data_->GetName() == cstr.str)
                break;
            data_ = data_->next;
        }

        if (data_) {
            data_->ref.fetch_add(1);
            return;
        }

        data_ = new Data;
        data_->ref.store(1);
        data_->cstr = cstr.str;
        data_->hash = hash;
        data_->idx = idx;
        data_->next = s_table_[idx];
        data_->prev = nullptr;

        if (s_table_[idx]) {
            s_table_[idx]->prev = data_;
        }

        s_table_[idx] = data_;
    }
    StringName::~StringName ()
    {
        unref();
    }
    StringName &StringName::operator= (const StringName &other)
    {
        if (this != other) {
            unref();
            if (other.data_) {
                other.data_->ref.fetch_add(1);
                data_ = other.data_;
            }
        }
        return *this;
    }
    void StringName::unref() {
        if (data_ && data_->ref.fetch_sub(1) == 1) {
            LOCK;
            if (data_->prev) {
                data_->prev->next = data_->next;
            }
            else {
                assert (data_ == s_table_[data_->idx]);
                s_table_[data_->idx] = data_->next;
            }

            if (data_->next)
                data_->next->prev = data_->prev;
            
            delete data_;
        }
        data_ = nullptr;
    }
    bool operator== (const std::string &p_name, const StringName &p_string_name)
    {
        return p_name == p_string_name.operator std::string();
    }
    bool operator!= (const std::string &p_name, const StringName &p_string_name)
    {
        return p_name != p_string_name.operator std::string();
    }
    bool operator== (const char *p_name, const StringName &p_string_name)
    {
        return p_name == p_string_name.operator std::string();
    }
    bool operator!= (const char *p_name, const StringName &p_string_name)
    {
        return p_name != p_string_name.operator std::string();
    }

    StaticCString StaticCString::create (const char *pstr)
    {
        StaticCString r{ .str = pstr };
        return r;
    }
    StaticCString StaticCString::create (std::string_view sv)
    {
        return create(sv.data());
    }
}  // namespace Meta
