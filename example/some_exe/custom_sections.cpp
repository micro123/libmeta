#include "custom_sections.hpp"
#include <cstdint>

EMPTY_SECTION (reg, FuncReg);

void call_all(const FuncReg* begin, const FuncReg* end)
{
    const auto cnt = end - begin;
    for (auto x = begin; x < end; ++x)
    {
        if (x->func)
            x->func ((void*)(uintptr_t)x->priority);
    }
}
