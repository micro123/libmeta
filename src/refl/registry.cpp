#include "refl/registry.hpp"
#include <mutex>
#include <unordered_map>
#include "refl/type.hpp"
#include "utility/constexpr.hpp"

#include "basic_types.hpp"

class Meta::Registry::Private
{
public:
    std::unordered_map<Meta::TypeId, Meta::TypePtr> db;
    std::recursive_mutex                            mtx;
    using lock_guard = std::lock_guard<decltype (mtx)>;

    lock_guard AutoLock ()
    {
        return lock_guard {mtx};
    }
};

Meta::Registry::Registry () : d (new Private)
{
    details::RegisterFundamentalTypes(this);
}

Meta::Registry::~Registry ()
{
    delete d;
}

Meta::Registry &Meta::Registry::Instance ()
{
    static Registry instance;
    return instance;
}

bool Meta::Registry::RegisterType (TypePtr type, TypeId tid)
{
    auto l  = d->AutoLock ();
    auto it = d->db.find (tid);
    if (it != d->db.end ())
        return false;
    d->db.insert ({tid, type});
    return true;
}

Meta::TypePtr Meta::Registry::Get (TypeId tid)
{
    auto l  = d->AutoLock ();
    auto it = d->db.find (tid);
    if (it != d->db.end ())
        return it->second;
    return {};
}
