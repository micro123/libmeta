#ifndef EXPORTS_H
#define EXPORTS_H

#if __cplusplus
#define C_LINKAGE extern "C"
#define C_BEGIN C_LINKAGE {
#define C_END   }
#endif

#if _MSC_VER
#define API_EXPORT __declspec(dllexport)
#define API_IMPORT __declspec(dllimport)
#elif __GNUC__
#define API_EXPORT __attribute__((visibility("default")))
#define API_IMPORT
#else
#define API_EXPORT
#define API_IMPORT
#endif

#if defined(LIBMETA_LIBRARY)
#define LIBMETA_API API_EXPORT
#else
#define LIBMETA_API API_IMPORT
#endif

C_BEGIN
#include "defines.h"
C_END

#endif /* EXPORTS_H */
