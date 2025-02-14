#ifndef CUSTOM_SECTIONS_HPP
#define CUSTOM_SECTIONS_HPP

#define CONCAT_(x,y) x##y
#define CONCAT(x,y) CONCAT_(x,y)
#define VARNAME(prefix) CONCAT(prefix##_, CONCAT(__LINE__, __COUNTER__))
#define STRINGIFY(x) #x

#if _MSC_VER

#define STORE_SECTION_(name) __declspec(allocate(name))
#define DEFINE_SECTION(name) __pragma(section(STRINGIFY(name##$a))) __pragma(section(STRINGIFY(name##$u))) __pragma(section(STRINGIFY(name##$z)))
#define STORE_SECTION(name) STORE_SECTION_(STRINGIFY(name##$u))
#define ALIGNED(N) __declspec(align(N))
#define SECTION_VARIABLES(name, type) \
    extern const type STORE_SECTION_(STRINGIFY(name##$a)) __start_##name; \
    extern const type STORE_SECTION_(STRINGIFY(name##$z)) __stop_##name
#define EMPTY_SECTION(name, type) \
    const type __declspec(allocate(STRINGIFY(name##$a))) ALIGNED(8) __start_##name {}; \
    const type __declspec(allocate(STRINGIFY(name##$z))) ALIGNED(8) __stop_##name {}

#elif __GNUC__ || __clang__

#else
#error "Unsupported compiler"
#endif

struct ALIGNED(sizeof(void*)) FuncReg {
    void(*func)(void*);
    int priority;

    FuncReg(void(*f)(void*) = nullptr, int p = 999) : func(f), priority(p) {}
};

DEFINE_SECTION (reg)
SECTION_VARIABLES(reg, FuncReg);
void call_all(const FuncReg* begin, const FuncReg* end);


#endif /* CUSTOM_SECTIONS_HPP */
