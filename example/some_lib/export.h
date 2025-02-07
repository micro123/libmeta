#ifndef EXPORT_H
#define EXPORT_H

#include "exports.h"

#if defined(LIB_EXPORT)
#define LIB_API API_EXPORT
#else
#define LIB_API API_IMPORT
#endif

#endif /* EXPORT_H */
