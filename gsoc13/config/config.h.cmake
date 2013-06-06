/**/
/* config.h.  Generated by cmake from config.h.cmake.  */

/* Define to 1 if you want to use `libcurl' in httpIO. */
#cmakedefine EXV_USE_CURL 1

/* Define to 1 if you have the `alarm' function. */
#cmakedefine EXV_HAVE_ALARM

/* Define to 1 if you have the declaration of `strerror_r', and to 0 if you
   don't. */
#cmakedefine EXV_HAVE_DECL_STRERROR_R 1

/* Define to 1 if you have the `gmtime_r' function. */
#cmakedefine EXV_HAVE_GMTIME_R 1

/* Define to 1 if you have the <inttypes.h> header file. */
#cmakedefine EXV_HAVE_INTTYPES_H 1

/* Define to 1 if you have the <libintl.h> header file. */
#cmakedefine EXV_HAVE_LIBINTL_H 1

#ifndef EXV_COMMERCIAL_VERSION

/* Define to 1 to enable translation of Nikon lens names. */
#cmakedefine EXV_HAVE_LENSDATA 1

/* Define to 1 if translation of program messages to the user's
   native language is requested. */
#cmakedefine EXV_ENABLE_NLS 1

/* Define to 1 if you have the `iconv' function. */
#cmakedefine EXV_HAVE_ICONV 1

/* Define to 1 to enable conversion of UCS2 encoded Windows tags to UTF-8. */
#cmakedefine EXV_HAVE_PRINTUCS2 1

#endif /* !EXV_COMMERCIAL_VERSION */

/* Define to `const' or to empty, depending on the second argument of `iconv'. */
#cmakedefine ICONV_ACCEPTS_CONST_INPUT 1
#if defined ICONV_ACCEPTS_CONST_INPUT
#define EXV_ICONV_CONST const
#else
#define EXV_ICONV_CONST
#endif

/* Define to 1 if you have the <libintl.h> header file. */
#cmakedefine EXV_HAVE_LIBINTL_H 1

/* Define to 1 if your system has a GNU libc compatible `malloc' function, and
   to 0 otherwise. */
#cmakedefine EXV_HAVE_MALLOC 1

/* Define to 1 if you have the <malloc.h> header file. */
#cmakedefine EXV_HAVE_MALLOC_H 1

/* Define to 1 if you have the <memory.h> header file. */
#cmakedefine EXV_HAVE_MEMORY_H 1

/* Define to 1 if you have the `memset' function. */
#cmakedefine EXV_HAVE_MEMSET 1

/* Define to 1 if your system has a GNU libc compatible `realloc' function,
   and to 0 otherwise. */
#cmakedefine EXV_HAVE_REALLOC 1

/* Define to 1 if stdbool.h conforms to C99. */
#cmakedefine EXV_HAVE_STDBOOL_H 1

/* Define to 1 if you have the <stdint.h> header file. */
#cmakedefine EXV_HAVE_STDINT_H 1

/* Define to 1 if you have the <stdlib.h> header file. */
#cmakedefine EXV_HAVE_STDLIB_H 1

/* Define to 1 if you have the `strchr' function. */
#cmakedefine EXV_HAVE_STRCHR 1

/* Define to 1 if you have the `strerror' function. */
#cmakedefine EXV_HAVE_STRERROR 1

/* Define to 1 if you have the `strerror_r' function. */
#cmakedefine EXV_HAVE_STRERROR_R 1

/* Define to 1 if you have the <strings.h> header file. */
#cmakedefine EXV_HAVE_STRINGS_H 1

/* Define to 1 if you have the <string.h> header file. */
#cmakedefine EXV_HAVE_STRING_H 1

/* Define to 1 if you have the `strtol' function. */
#cmakedefine EXV_HAVE_STRTOL 1

/* Define to 1 if you have the `mmap' function. */
#cmakedefine EXV_HAVE_MMAP 1

/* Define to 1 if you have the `munmap' function. */
#cmakedefine EXV_HAVE_MUNMAP 1

/* Define to 1 if you have the <sys/stat.h> header file. */
#cmakedefine EXV_HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/time.h> header file. */
#cmakedefine EXV_HAVE_SYS_TIME_H 1

/* Define to 1 if you have the <sys/types.h> header file. */
#cmakedefine EXV_HAVE_SYS_TYPES_H 1

/* Define to 1 if you have the `timegm' function. */
#cmakedefine EXV_HAVE_TIMEGM 1

/* Define to 1 if you have the <unistd.h> header file. */
#cmakedefine EXV_HAVE_UNISTD_H 1

/* Define to 1 if you have the `vprintf' function. */
#cmakedefine EXV_HAVE_VPRINTF 1

/* Define to 1 if you have the <wchar.h> header file. */
#cmakedefine EXV_HAVE_WCHAR_H 1

/* Define to 1 if you have the <sys/mman.h> header file. */
#cmakedefine EXV_HAVE_SYS_MMAN_H 1

/* Define to 1 if you have the `zlib' library. */
#cmakedefine EXV_HAVE_LIBZ 1

/* Define to 1 if you have the <process.h> header file. */
#cmakedefine EXV_HAVE_PROCESS_H 1

/* Define to 1 if you have the Adobe XMP Toolkit. */
#cmakedefine EXV_HAVE_XMP_TOOLKIT 1

/* Define to 1 if the system has the type `_Bool'. */
#cmakedefine EXV_HAVE__BOOL 1

/* Define to 1 if `lstat' dereferences a symlink specified with a trailing
   slash. */
#cmakedefine EXV_LSTAT_FOLLOWS_SLASHED_SYMLINK 1

/* Define to the address where bug reports for this package should be sent. */
#cmakedefine EXV_PACKAGE_BUGREPORT "@PACKAGE_BUGREPORT@"

/* Define to the name of this package. */
#cmakedefine EXV_PACKAGE "@PACKAGE@"

/* Define to the full name of this package. */
#cmakedefine EXV_PACKAGE_NAME "@PACKAGE_NAME@"

/* Define to the full name and version of this package. */
#cmakedefine EXV_PACKAGE_STRING "@PACKAGE_STRING@"

/* Define to the one symbol short name of this package. */
#cmakedefine EXV_PACKAGE_TARNAME "@PACKAGE_TARNAME@"

/* Define to the version of this package. */
#cmakedefine EXV_PACKAGE_VERSION "@PACKAGE_VERSION@"

/* Define to 1 if you have the ANSI C header files. */
#cmakedefine STDC_HEADERS 1

/* Define to 1 if strerror_r returns char *. */
#cmakedefine STRERROR_R_CHAR_P 1

/* Define to 1 if you can safely include both <sys/time.h> and <time.h>. */
#define TIME_WITH_SYS_TIME 1

/* Define to 1 if your <sys/time.h> declares `struct tm'. */
#cmakedefine TM_IN_SYS_TIME 1

/* Define to empty if `const' does not conform to ANSI C. */
#cmakedefine const

/* Define to `__inline__' or `__inline' if that's what the C compiler
   calls it, or to nothing if 'inline' is not supported under any name.  */
#ifndef __cplusplus
#cmakedefine inline
#endif

/* Define to rpl_malloc if the replacement function should be used. */
#cmakedefine malloc

/* Define to `int' if <sys/types.h> does not define. */
#cmakedefine HAVE_PID_T 1

/* On Microsoft compilers pid_t has to be set to int. */
#ifndef HAVE_PID_T
typedef int pid_t;
#endif

/* Define to rpl_realloc if the replacement function should be used. */
#cmakedefine realloc

/* Define to `unsigned' if <sys/types.h> does not define. */
#cmakedefine size_t

#if defined(__CYGWIN32__) && !defined(__CYGWIN__)
   /* For backwards compatibility with Cygwin b19 and
      earlier, we define __CYGWIN__ here, so that
      we can rely on checking just for that macro. */
#define __CYGWIN__  __CYGWIN32__
#define EXV_HAVE_GXXCLASSVISIBILITY
#endif
     
/* File path seperator */
#if defined(WIN32) && !defined(__CYGWIN__)
#define EXV_SEPERATOR_STR "\\"
#define EXV_SEPERATOR_CHR '\\'
#else
#define EXV_SEPERATOR_STR "/"
#define EXV_SEPERATOR_CHR '/'
#endif

/* Shared library support, see http://gcc.gnu.org/wiki/Visibility */
#if defined(WIN32) && !defined(__CYGWIN__)
# define EXV_IMPORT __declspec(dllimport)
# define EXV_EXPORT __declspec(dllexport)
# define EXV_DLLLOCAL
# define EXV_DLLPUBLIC
#else
# ifdef EXV_HAVE_GXXCLASSVISIBILITY
#  define EXV_IMPORT __attribute__ ((visibility("default")))
#  define EXV_EXPORT __attribute__ ((visibility("default")))
#  define EXV_DLLLOCAL __attribute__ ((visibility("hidden")))
#  define EXV_DLLPUBLIC __attribute__ ((visibility("default")))
# else
#  define EXV_IMPORT
#  define EXV_EXPORT
#  define EXV_DLLLOCAL
#  define EXV_DLLPUBLIC
# endif /* ! EXV_HAVE_GXXCLASSVISIBILITY */
#endif /* ! WIN32 */

/* Define EXIV2API for DLL builds */
#ifdef EXV_HAVE_DLL
# ifdef EXV_BUILDING_LIB
#  define EXIV2API EXV_EXPORT
# else
#  define EXIV2API EXV_IMPORT
# endif /* ! EXV_BUILDING_LIB */
#else
# define EXIV2API
#endif /* ! EXV_HAVE_DLL */

/*
  Visual Studio C++ 2005 (8.0)
  Disable warnings about 'deprecated' standard functions
  See, eg. http://www.informit.com/guides/content.asp?g=cplusplus&seqNum=259
*/
#if defined(_MSC_VER) && (_MSC_VER >= 1400)
# pragma warning(disable : 4996 4251)
#endif
