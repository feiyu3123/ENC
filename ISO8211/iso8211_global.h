#ifndef ISO8211_GLOBAL_H
#define ISO8211_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(ISO8211_LIBRARY)
#  define ISO8211SHARED_EXPORT Q_DECL_EXPORT
#else
#  define ISO8211SHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // ISO8211_GLOBAL_H
