#ifndef LIBTRKQT_GLOBAL_H
#define LIBTRKQT_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(LIBTRKQT_LIBRARY)
#  define LIBTRKQTSHARED_EXPORT Q_DECL_EXPORT
#else
#  define LIBTRKQTSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // LIBTRKQT_GLOBAL_H
