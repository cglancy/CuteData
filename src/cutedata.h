#ifndef CUTEDATA_H
#define CUTEDATA_H

#include <QtGlobal>

#ifdef Q_OS_WIN32

#ifdef CUTEDATA_EXPORTS
#define CUTEDATA_API __declspec(dllexport)
#else
#define CUTEDATA_API __declspec(dllimport)
#endif

#else

#define CUTEDATA_API __attribute__((visibility("default")))

#endif

#endif // CUTEDATA_H
