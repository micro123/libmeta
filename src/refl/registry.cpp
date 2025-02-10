#include "refl/registry.hpp"
#include <mutex>
#include <unordered_map>
#include "basic_types.hpp"
#include "refl/type.hpp"
#include "utility/constexpr.hpp"

class Meta::Registry::Private
{
public:
    std::unordered_map<Meta::TypeId, Meta::TypePtr> db_tid2type;
    std::unordered_map<sview, Meta::TypePtr>        db_name2type;
    std::recursive_mutex                            mtx;
    using lock_guard = std::lock_guard<decltype (mtx)>;

    lock_guard AutoLock ()
    {
        return lock_guard {mtx};
    }
};

Meta::Registry::Registry () : d (new Private)
{
    
}

Meta::Registry::~Registry ()
{
    delete d;
}

Meta::Registry &Meta::Registry::Instance ()
{
    static Registry *instance = nullptr;
    if (!instance) {
        static Registry real;
        instance = &real;
        details::RegisterFundamentalTypes (instance);
    }
    return *instance;
}

bool Meta::Registry::RegisterType (TypePtr type, TypeId tid)
{
    assert (type); // 这个不能为空
    auto l  = d->AutoLock ();
    //  需要确保两个都没有重复
    auto const it1 = d->db_tid2type.find(tid);
    auto const it2 = d->db_name2type.find(type->Name());

    if (it1 != end(d->db_tid2type))
    {
        assert(0 && "duplicated type id!!");
        return false;
    }

    if (it2 != end(d->db_name2type))
    {
        assert(0 && "duplicated type name!!");
        return false;
    }

    d->db_tid2type.insert({tid, type});
    d->db_name2type.insert({type->Name(), type});

    return true;
}

Meta::TypePtr Meta::Registry::Get (TypeId tid)
{
    auto l  = d->AutoLock ();
    auto it = d->db_tid2type.find (tid);
    if (it != d->db_tid2type.end ())
        return it->second;
    return {};
}

Meta::TypePtr Meta::Registry::Get (const str name)
{
    auto l  = d->AutoLock ();
    auto it = d->db_name2type.find (name);
    if (it != d->db_name2type.end ())
        return it->second;
    return {};

}

void Meta::Registry::VisitTypes (const std::function<bool (TypeId, TypePtr)> &visitor) const {
    if (!visitor)
        return;
    auto l = d->AutoLock();
    auto it = begin(d->db_tid2type);
    while (it != end(d->db_tid2type)) {
        if (!visitor(it->first, it->second))
            break;
        ++it;
    }
}
