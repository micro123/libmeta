#ifndef LIBMETA_META_PARSE_H
#define LIBMETA_META_PARSE_H

#ifdef __META_CODEGEN__

#define ANNO_(x) __attribute__((annotate(x)))
#define ANNO(x) ANNO_(x)
#define META(...)   ANNO(#__VA_ARGS__) ANNO("Enabled")
#define CLASS(name, ...)  class  ANNO(#__VA_ARGS__) ANNO("Enabled") name
#define STRUCT(name, ...) struct ANNO(#__VA_ARGS__) ANNO("Enabled") name
#define UNION(name, ...)  union  ANNO(#__VA_ARGS__) ANNO("Enabled") name
#define ENUM(name, ...)   enum   ANNO(#__VA_ARGS__) ANNO("Enabled") name
#define REFL_BODY(...)

#else

#define META(...)
#define CLASS(name, ...)  class  name
#define STRUCT(name, ...) struct name
#define UNION(name, ...)  union  name
#define ENUM(name, ...)   enum   name
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
