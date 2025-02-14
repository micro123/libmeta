#include "custom_sections.hpp"

static void foo(void*ptr)
{
    int a = 0;
}

static const FuncReg STORE_SECTION(reg) ALIGNED(8) VARNAME(foo) { &foo, 1 };
static const FuncReg STORE_SECTION(reg) ALIGNED(8) VARNAME(foo) { &foo, 2 };
static const FuncReg STORE_SECTION(reg) ALIGNED(8) VARNAME(foo) { &foo, 3 };
static const FuncReg STORE_SECTION(reg) ALIGNED(8) VARNAME(foo) { &foo, 4 };
