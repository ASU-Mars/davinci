/* H5pubconf.h is adapted from UNIX platform and manually maintained on the windows platform. */

/*#define H5_HAVE_TM_ZONE 1 windows do not use this constant.*/  
#define H5_MALLOC_WORKS 1
#define H5_STDC_HEADERS 1
#define H5_HAVE_ATTRIBUTE 1
#undef H5_HAVE_ATTRIBUTE
#define H5_HAVE_LARGE_HSIZET 1 
#ifdef __MWERKS__ 
#define H5_PRINTF_LL_WIDTH "ll" 
#else 
#define H5_PRINTF_LL_WIDTH "I64" 
#endif
#define H5_HAVE___int64
#define H5_SIZEOF___INT64 8 
#define H5_SIZEOF_CHAR 1
#define H5_SIZEOF_DOUBLE 8
#define H5_SIZEOF_FLOAT 4
#define H5_SIZEOF_INT 4
#define H5_SIZEOF_INT16_T 0
#define H5_SIZEOF_INT32_T 0
#define H5_SIZEOF_INT64_T 0
#define H5_SIZEOF_INT8_T 0
#define H5_SIZEOF_INT_FAST16_T 0
#define H5_SIZEOF_INT_FAST32_T 0
#define H5_SIZEOF_INT_FAST64_T 0
#define H5_SIZEOF_INT_FAST8_T 0
#define H5_SIZEOF_INT_LEAST16_T 0
#define H5_SIZEOF_INT_LEAST32_T 0
#define H5_SIZEOF_INT_LEAST64_T 0
#define H5_SIZEOF_INT_LEAST8_T 0
#define H5_SIZEOF_LONG 4
#define H5_SIZEOF_LONG_DOUBLE 8
#define H5_SIZEOF_OFF_T 4
#define H5_SIZEOF_SHORT 2
#define H5_SIZEOF_SIZE_T 4
#define H5_SIZEOF_OFF_T 4
#define H5_SIZEOF_SSIZE_T 0
#define H5_SIZEOF_UINT16_T 0
#define H5_SIZEOF_UINT32_T 0
#define H5_SIZEOF_UINT8_T 0
#define H5_SIZEOF_UINT_FAST16_T 0
#define H5_SIZEOF_UINT_FAST32_T 0
#define H5_SIZEOF_UINT_FAST64_T 0
#define H5_SIZEOF_UINT_FAST8_T 0
#define H5_SIZEOF_UINT_LEAST16_T 0
#define H5_SIZEOF_UINT_LEAST32_T 0
#define H5_SIZEOF_UINT_LEAST64_T 0
#define H5_SIZEOF_UINT_LEAST8_T 0
#define H5_HAVE_DIFFTIME 1
#define H5_HAVE_FORK 1
#define H5_HAVE_GETHOSTNAME 1
#define H5_HAVE_IOCTL 1
#define H5_HAVE_LONGJMP 1
#define H5_HAVE_SIGACTION 1
#define H5_HAVE_SIGNAL 1
#define H5_HAVE__SNPRINTF 1
#define H5_HAVE_STRDUP 1
#define H5_HAVE_SYSTEM 1
#define H5_HAVE__VSNPRINTF 1
#define  H5_HAVE_IO_H 1
#define H5_HAVE_SETJMP_H 1
#define H5_HAVE_STDDEF_H 1
#define H5_HAVE_SYS_STAT_H 1
#define H5_HAVE_SYS_TIMEB 1
#define H5_HAVE_SYS_TYPES_H 1
#define H5_HAVE_WINSOCK_H 1
#define H5_HAVE_ZLIB_H 1
#define H5_HAVE_LIBZ 1
#define H5_HAVE_COMPRESS2 1
/*#define ssize_t long    */
/*#if defined(__cplusplus) && defined(inline)
#undef inline
#endif
*/
#if defined(__MWERKS__) || defined(__cplusplus)
#define H5_inline inline
#else
#define H5_inline __inline
#endif
