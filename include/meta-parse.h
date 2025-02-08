#ifndef LIBMETA_META_PARSE_H
#define LIBMETA_META_PARSE_H

#ifdef __META_CODEGEN__
#define META(...)   __attribute__((annotate(#__VA_ARGS__)))
#define CLASS(name, ...) class __attribute__((annotate(#__VA_ARGS__))) name
#define STRUCT(name, ...) struct __attribute__((annotate(#__VA_ARGS__))) name
#define REFL_BODY(...)
#else
#define META(...)
#define CLASS(name, ...) class name
#define STRUCT(name, ...) struct name
#define REFL_BODY(name) \
private:\
    friend class Meta::CodeGenFor<name>;\
public:

namespace Meta {
    template <typename T>
    struct CodeGenFor {
        static void Register();
    };
}
#endif

#endif /* LIBMETA_META_PARSE_H */
