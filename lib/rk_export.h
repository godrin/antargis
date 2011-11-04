#ifndef __RK_EXPORT_H
#define __RK_EXPORT_H

#ifdef WIN32
#ifdef AGIMPORT
#define AGEXPORT __declspec(dllimport)
#else
#undef AGEXPORT
#define AGEXPORT __declspec(dllexport)
#endif // AGIMPORT
#else

#undef AGEXPORT
#define AGEXPORT

#endif // WIN32

#ifdef SWIG
#undef AGEXPORT
#define AGEXPORT
#endif

#endif
