#include "refl/registry.hpp"
#include "refl/type.hpp"
#include "utility/constexpr.hpp"

#include <mutex>
#include <unordered_map>

#define REGISTER(type) \
    do { \
        auto t = new Meta::Type{#type, sizeof(type), Meta::CalcTypeFlags<type>()}; \
        RegisterType(t, Meta::GetTypeId<type>()); \
    } while(0)

class Meta::Registry::Private {
public:
    std::unordered_map<Meta::TypeId, Meta::Type*> db;   
    std::recursive_mutex mtx;
    using lock_guard = std::lock_guard<decltype(mtx)>;

    lock_guard AutoLock() {
        return lock_guard{mtx};
    }
};

Meta::Registry::Registry(): d(new Private) {
    FUNDAMENTAL_TYPES(REGISTER);
}

Meta::Registry::~Registry() {
    delete d;
}

Meta::Registry &Meta::Registry::Instance() {
    static Registry instance;
    return instance;
}

bool Meta::Registry::RegisterType(Type *type, TypeId tid) {
    auto l = d->AutoLock();
    auto it = d->db.find(tid);
    if (it != d->db.end())
        return false;
    d->db.insert({tid, type});
    return true;
}

Meta::Type *Meta::Registry::Get(TypeId tid) {
    auto l = d->AutoLock();
    auto it = d->db.find(tid);
    if (it != d->db.end())
        return it->second;
    return nullptr;
}
