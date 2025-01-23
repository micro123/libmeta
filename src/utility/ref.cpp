#include "utility/ref.hpp"

namespace Meta::details {
    RefCountedBase::RefCountedBase(): ref_count_{1} {}
    
    RefCountedBase::~RefCountedBase() = default;
    
    void RefCountedBase::AddRef() {
        ref_count_.fetch_add(1);
    }

    void RefCountedBase::UnRef() {
        if (ref_count_.fetch_sub(1) == 1) {
            delete this;
        }
    }
}
