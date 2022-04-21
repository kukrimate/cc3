
extern void __assert_fail (const char *__assertion, const char *__file,
      unsigned int __line, const char *__function)
     ;
extern void __assert_perror_fail (int __errnum, const char *__file,
      unsigned int __line, const char *__function)
     ;
extern void __assert (const char *__assertion, const char *__file, int __line)
     ;


extern int *__errno_location (void) ;

// FIXME: implement this
typedef void *__builtin_va_list;

typedef __builtin_va_list __gnuc_va_list;
typedef __gnuc_va_list va_list;

typedef long unsigned int size_t;
typedef unsigned char __u_char;
typedef unsigned short int __u_short;
typedef unsigned int __u_int;
typedef unsigned long int __u_long;
typedef signed char __int8_t;
typedef unsigned char __uint8_t;
typedef signed short int __int16_t;
typedef unsigned short int __uint16_t;
typedef signed int __int32_t;
typedef unsigned int __uint32_t;
typedef signed long int __int64_t;
typedef unsigned long int __uint64_t;
typedef __int8_t __int_least8_t;
typedef __uint8_t __uint_least8_t;
typedef __int16_t __int_least16_t;
typedef __uint16_t __uint_least16_t;
typedef __int32_t __int_least32_t;
typedef __uint32_t __uint_least32_t;
typedef __int64_t __int_least64_t;
typedef __uint64_t __uint_least64_t;
typedef long int __quad_t;
typedef unsigned long int __u_quad_t;
typedef long int __intmax_t;
typedef unsigned long int __uintmax_t;
typedef unsigned long int __dev_t;
typedef unsigned int __uid_t;
typedef unsigned int __gid_t;
typedef unsigned long int __ino_t;
typedef unsigned long int __ino64_t;
typedef unsigned int __mode_t;
typedef unsigned long int __nlink_t;
typedef long int __off_t;
typedef long int __off64_t;
typedef int __pid_t;
typedef struct { int __val[2]; } __fsid_t;
typedef long int __clock_t;
typedef unsigned long int __rlim_t;
typedef unsigned long int __rlim64_t;
typedef unsigned int __id_t;
typedef long int __time_t;
typedef unsigned int __useconds_t;
typedef long int __suseconds_t;
typedef int __daddr_t;
typedef int __key_t;
typedef int __clockid_t;
typedef void * __timer_t;
typedef long int __blksize_t;
typedef long int __blkcnt_t;
typedef long int __blkcnt64_t;
typedef unsigned long int __fsblkcnt_t;
typedef unsigned long int __fsblkcnt64_t;
typedef unsigned long int __fsfilcnt_t;
typedef unsigned long int __fsfilcnt64_t;
typedef long int __fsword_t;
typedef long int __ssize_t;
typedef long int __syscall_slong_t;
typedef unsigned long int __syscall_ulong_t;
typedef __off64_t __loff_t;
typedef char *__caddr_t;
typedef long int __intptr_t;
typedef unsigned int __socklen_t;
typedef int __sig_atomic_t;
typedef struct
{
  int __count;
  union
  {
    unsigned int __wch;
    char __wchb[4];
  } __value;
} __mbstate_t;
typedef struct _G_fpos_t
{
  __off_t __pos;
  __mbstate_t __state;
} __fpos_t;
typedef struct _G_fpos64_t
{
  __off64_t __pos;
  __mbstate_t __state;
} __fpos64_t;
struct _IO_FILE;
typedef struct _IO_FILE __FILE;
struct _IO_FILE;
typedef struct _IO_FILE FILE;
struct _IO_FILE;
struct _IO_marker;
struct _IO_codecvt;
struct _IO_wide_data;
typedef void _IO_lock_t;
struct _IO_FILE
{
  int _flags;
  char *_IO_read_ptr;
  char *_IO_read_end;
  char *_IO_read_base;
  char *_IO_write_base;
  char *_IO_write_ptr;
  char *_IO_write_end;
  char *_IO_buf_base;
  char *_IO_buf_end;
  char *_IO_save_base;
  char *_IO_backup_base;
  char *_IO_save_end;
  struct _IO_marker *_markers;
  struct _IO_FILE *_chain;
  int _fileno;
  int _flags2;
  __off_t _old_offset;
  unsigned short _cur_column;
  signed char _vtable_offset;
  char _shortbuf[1];
  _IO_lock_t *_lock;
  __off64_t _offset;
  struct _IO_codecvt *_codecvt;
  struct _IO_wide_data *_wide_data;
  struct _IO_FILE *_freeres_list;
  void *_freeres_buf;
  size_t __pad5;
  int _mode;
  char _unused2[15 * sizeof (int) - 4 * sizeof (void *) - sizeof (size_t)];
};
typedef __off_t off_t;
typedef __ssize_t ssize_t;
typedef __fpos_t fpos_t;
extern FILE *stdin;
extern FILE *stdout;
extern FILE *stderr;
extern int remove (const char *__filename) ;
extern int rename (const char *__old, const char *__new) ;
extern int renameat (int __oldfd, const char *__old, int __newfd,
       const char *__new) ;
extern FILE *tmpfile (void) ;
extern char *tmpnam (char *__s) ;
extern char *tmpnam_r (char *__s) ;
extern char *tempnam (const char *__dir, const char *__pfx)
     ;
extern int fclose (FILE *__stream);
extern int fflush (FILE *__stream);
extern int fflush_unlocked (FILE *__stream);
extern FILE *fopen (const char *restrict __filename,
      const char *restrict __modes) ;
extern FILE *freopen (const char *restrict __filename,
        const char *restrict __modes,
        FILE *restrict __stream) ;
extern FILE *fdopen (int __fd, const char *__modes) ;
extern FILE *fmemopen (void *__s, size_t __len, const char *__modes)
  ;
extern FILE *open_memstream (char **__bufloc, size_t *__sizeloc) ;
extern void setbuf (FILE *restrict __stream, char *restrict __buf) ;
extern int setvbuf (FILE *restrict __stream, char *restrict __buf,
      int __modes, size_t __n) ;
extern void setbuffer (FILE *restrict __stream, char *restrict __buf,
         size_t __size) ;
extern void setlinebuf (FILE *__stream) ;
extern int fprintf (FILE *restrict __stream,
      const char *restrict __format, ...);
extern int printf (const char *restrict __format, ...);
extern int sprintf (char *restrict __s,
      const char *restrict __format, ...) ;
extern int vfprintf (FILE *restrict __s, const char *restrict __format,
       __gnuc_va_list __arg);
extern int vprintf (const char *restrict __format, __gnuc_va_list __arg);
extern int vsprintf (char *restrict __s, const char *restrict __format,
       __gnuc_va_list __arg) ;
extern int snprintf (char *restrict __s, size_t __maxlen,
       const char *restrict __format, ...)
     ;
extern int vsnprintf (char *restrict __s, size_t __maxlen,
        const char *restrict __format, __gnuc_va_list __arg)
     ;
extern int vdprintf (int __fd, const char *restrict __fmt,
       __gnuc_va_list __arg)
     ;
extern int dprintf (int __fd, const char *restrict __fmt, ...)
     ;
extern int fscanf (FILE *restrict __stream,
     const char *restrict __format, ...) ;
extern int scanf (const char *restrict __format, ...) ;
extern int sscanf (const char *restrict __s,
     const char *restrict __format, ...) ;
extern int __isoc99_fscanf (FILE *restrict __stream,
       const char *restrict __format, ...) ;
extern int __isoc99_scanf (const char *restrict __format, ...) ;
extern int __isoc99_sscanf (const char *restrict __s,
       const char *restrict __format, ...) ;
extern int vfscanf (FILE *restrict __s, const char *restrict __format,
      __gnuc_va_list __arg)
     ;
extern int vscanf (const char *restrict __format, __gnuc_va_list __arg)
     ;
extern int vsscanf (const char *restrict __s,
      const char *restrict __format, __gnuc_va_list __arg)
     ;
extern int __isoc99_vfscanf (FILE *restrict __s,
        const char *restrict __format,
        __gnuc_va_list __arg) ;
extern int __isoc99_vscanf (const char *restrict __format,
       __gnuc_va_list __arg) ;
extern int __isoc99_vsscanf (const char *restrict __s,
        const char *restrict __format,
        __gnuc_va_list __arg) ;
extern int fgetc (FILE *__stream);
extern int getc (FILE *__stream);
extern int getchar (void);
extern int getc_unlocked (FILE *__stream);
extern int getchar_unlocked (void);
extern int fgetc_unlocked (FILE *__stream);
extern int fputc (int __c, FILE *__stream);
extern int putc (int __c, FILE *__stream);
extern int putchar (int __c);
extern int fputc_unlocked (int __c, FILE *__stream);
extern int putc_unlocked (int __c, FILE *__stream);
extern int putchar_unlocked (int __c);
extern int getw (FILE *__stream);
extern int putw (int __w, FILE *__stream);
extern char *fgets (char *restrict __s, int __n, FILE *restrict __stream)
     ;
extern __ssize_t __getdelim (char **restrict __lineptr,
                             size_t *restrict __n, int __delimiter,
                             FILE *restrict __stream) ;
extern __ssize_t getdelim (char **restrict __lineptr,
                           size_t *restrict __n, int __delimiter,
                           FILE *restrict __stream) ;
extern __ssize_t getline (char **restrict __lineptr,
                          size_t *restrict __n,
                          FILE *restrict __stream) ;
extern int fputs (const char *restrict __s, FILE *restrict __stream);
extern int puts (const char *__s);
extern int ungetc (int __c, FILE *__stream);
extern size_t fread (void *restrict __ptr, size_t __size,
       size_t __n, FILE *restrict __stream) ;
extern size_t fwrite (const void *restrict __ptr, size_t __size,
        size_t __n, FILE *restrict __s);
extern size_t fread_unlocked (void *restrict __ptr, size_t __size,
         size_t __n, FILE *restrict __stream) ;
extern size_t fwrite_unlocked (const void *restrict __ptr, size_t __size,
          size_t __n, FILE *restrict __stream);
extern int fseek (FILE *__stream, long int __off, int __whence);
extern long int ftell (FILE *__stream) ;
extern void rewind (FILE *__stream);
extern int fseeko (FILE *__stream, __off_t __off, int __whence);
extern __off_t ftello (FILE *__stream) ;
extern int fgetpos (FILE *restrict __stream, fpos_t *restrict __pos);
extern int fsetpos (FILE *__stream, const fpos_t *__pos);
extern void clearerr (FILE *__stream) ;
extern int feof (FILE *__stream) ;
extern int ferror (FILE *__stream) ;
extern void clearerr_unlocked (FILE *__stream) ;
extern int feof_unlocked (FILE *__stream) ;
extern int ferror_unlocked (FILE *__stream) ;
extern void perror (const char *__s);
extern int sys_nerr;
extern const char *const sys_errlist[];
extern int fileno (FILE *__stream) ;
extern int fileno_unlocked (FILE *__stream) ;
extern FILE *popen (const char *__command, const char *__modes) ;
extern int pclose (FILE *__stream);
extern char *ctermid (char *__s) ;
extern void flockfile (FILE *__stream) ;
extern int ftrylockfile (FILE *__stream) ;
extern void funlockfile (FILE *__stream) ;
extern int __uflow (FILE *);
extern int __overflow (FILE *, int);

typedef int wchar_t;

typedef enum
{
  P_ALL,
  P_PID,
  P_PGID
} idtype_t;
typedef float _Float32;
typedef double _Float64;
typedef double _Float32x;
typedef long double _Float64x;
typedef struct
  {
    int quot;
    int rem;
  } div_t;
typedef struct
  {
    long int quot;
    long int rem;
  } ldiv_t;
 typedef struct
  {
    long long int quot;
    long long int rem;
  } lldiv_t;
extern size_t __ctype_get_mb_cur_max (void) ;
extern double atof (const char *__nptr)
     ;
extern int atoi (const char *__nptr)
     ;
extern long int atol (const char *__nptr)
     ;
 extern long long int atoll (const char *__nptr)
     ;
extern double strtod (const char *restrict __nptr,
        char **restrict __endptr)
     ;
extern float strtof (const char *restrict __nptr,
       char **restrict __endptr) ;
extern long double strtold (const char *restrict __nptr,
       char **restrict __endptr)
     ;
extern long int strtol (const char *restrict __nptr,
   char **restrict __endptr, int __base)
     ;
extern unsigned long int strtoul (const char *restrict __nptr,
      char **restrict __endptr, int __base)
     ;

extern long long int strtoq (const char *restrict __nptr,
        char **restrict __endptr, int __base)
     ;

extern unsigned long long int strtouq (const char *restrict __nptr,
           char **restrict __endptr, int __base)
     ;

extern long long int strtoll (const char *restrict __nptr,
         char **restrict __endptr, int __base)
     ;

extern unsigned long long int strtoull (const char *restrict __nptr,
     char **restrict __endptr, int __base)
     ;
extern char *l64a (long int __n) ;
extern long int a64l (const char *__s)
     ;

typedef __u_char u_char;
typedef __u_short u_short;
typedef __u_int u_int;
typedef __u_long u_long;
typedef __quad_t quad_t;
typedef __u_quad_t u_quad_t;
typedef __fsid_t fsid_t;
typedef __loff_t loff_t;
typedef __ino_t ino_t;
typedef __dev_t dev_t;
typedef __gid_t gid_t;
typedef __mode_t mode_t;
typedef __nlink_t nlink_t;
typedef __uid_t uid_t;
typedef __pid_t pid_t;
typedef __id_t id_t;
typedef __daddr_t daddr_t;
typedef __caddr_t caddr_t;
typedef __key_t key_t;
typedef __clock_t clock_t;
typedef __clockid_t clockid_t;
typedef __time_t time_t;
typedef __timer_t timer_t;
typedef unsigned long int ulong;
typedef unsigned short int ushort;
typedef unsigned int uint;
typedef __int8_t int8_t;
typedef __int16_t int16_t;
typedef __int32_t int32_t;
typedef __int64_t int64_t;
typedef __uint8_t u_int8_t;
typedef __uint16_t u_int16_t;
typedef __uint32_t u_int32_t;
typedef __uint64_t u_int64_t;
typedef int register_t;
static __inline __uint16_t
__bswap_16 (__uint16_t __bsx)
{
  return ((__uint16_t) ((((__bsx) >> 8) & 0xff) | (((__bsx) & 0xff) << 8)));
}
static __inline __uint32_t
__bswap_32 (__uint32_t __bsx)
{
  return ((((__bsx) & 0xff000000u) >> 24) | (((__bsx) & 0x00ff0000u) >> 8) | (((__bsx) & 0x0000ff00u) << 8) | (((__bsx) & 0x000000ffu) << 24));
}
 static __inline __uint64_t
__bswap_64 (__uint64_t __bsx)
{
  return ((((__bsx) & 0xff00000000000000ull) >> 56) | (((__bsx) & 0x00ff000000000000ull) >> 40) | (((__bsx) & 0x0000ff0000000000ull) >> 24) | (((__bsx) & 0x000000ff00000000ull) >> 8) | (((__bsx) & 0x00000000ff000000ull) << 8) | (((__bsx) & 0x0000000000ff0000ull) << 24) | (((__bsx) & 0x000000000000ff00ull) << 40) | (((__bsx) & 0x00000000000000ffull) << 56));
}
static __inline __uint16_t
__uint16_identity (__uint16_t __x)
{
  return __x;
}
static __inline __uint32_t
__uint32_identity (__uint32_t __x)
{
  return __x;
}
static __inline __uint64_t
__uint64_identity (__uint64_t __x)
{
  return __x;
}
typedef struct
{
  unsigned long int __val[(1024 / (8 * sizeof (unsigned long int)))];
} __sigset_t;
typedef __sigset_t sigset_t;
struct timeval
{
  __time_t tv_sec;
  __suseconds_t tv_usec;
};
struct timespec
{
  __time_t tv_sec;
  __syscall_slong_t tv_nsec;
};
typedef __suseconds_t suseconds_t;
typedef long int __fd_mask;
typedef struct
  {
    __fd_mask __fds_bits[1024 / (8 * (int) sizeof (__fd_mask))];
  } fd_set;
typedef __fd_mask fd_mask;

extern int select (int __nfds, fd_set *restrict __readfds,
     fd_set *restrict __writefds,
     fd_set *restrict __exceptfds,
     struct timeval *restrict __timeout);
extern int pselect (int __nfds, fd_set *restrict __readfds,
      fd_set *restrict __writefds,
      fd_set *restrict __exceptfds,
      const struct timespec *restrict __timeout,
      const __sigset_t *restrict __sigmask);

typedef __blksize_t blksize_t;
typedef __blkcnt_t blkcnt_t;
typedef __fsblkcnt_t fsblkcnt_t;
typedef __fsfilcnt_t fsfilcnt_t;
typedef struct __pthread_internal_list
{
  struct __pthread_internal_list *__prev;
  struct __pthread_internal_list *__next;
} __pthread_list_t;
typedef struct __pthread_internal_slist
{
  struct __pthread_internal_slist *__next;
} __pthread_slist_t;
struct __pthread_mutex_s
{
  int __lock;
  unsigned int __count;
  int __owner;
  unsigned int __nusers;
  int __kind;
  short __spins;
  short __elision;
  __pthread_list_t __list;
};
struct __pthread_rwlock_arch_t
{
  unsigned int __readers;
  unsigned int __writers;
  unsigned int __wrphase_futex;
  unsigned int __writers_futex;
  unsigned int __pad3;
  unsigned int __pad4;
  int __cur_writer;
  int __shared;
  signed char __rwelision;
  unsigned char __pad1[7];
  unsigned long int __pad2;
  unsigned int __flags;
};
struct __pthread_cond_s
{
  union
  {
    unsigned long long int __wseq;
    struct
    {
      unsigned int __low;
      unsigned int __high;
    } __wseq32;
  };
  union
  {
    unsigned long long int __g1_start;
    struct
    {
      unsigned int __low;
      unsigned int __high;
    } __g1_start32;
  };
  unsigned int __g_refs[2] ;
  unsigned int __g_size[2];
  unsigned int __g1_orig_size;
  unsigned int __wrefs;
  unsigned int __g_signals[2];
};
typedef unsigned long int pthread_t;
typedef union
{
  char __size[4];
  int __align;
} pthread_mutexattr_t;
typedef union
{
  char __size[4];
  int __align;
} pthread_condattr_t;
typedef unsigned int pthread_key_t;
typedef int pthread_once_t;
union pthread_attr_t
{
  char __size[56];
  long int __align;
};
typedef union pthread_attr_t pthread_attr_t;
typedef union
{
  struct __pthread_mutex_s __data;
  char __size[40];
  long int __align;
} pthread_mutex_t;
typedef union
{
  struct __pthread_cond_s __data;
  char __size[48];
  long long int __align;
} pthread_cond_t;
typedef union
{
  struct __pthread_rwlock_arch_t __data;
  char __size[56];
  long int __align;
} pthread_rwlock_t;
typedef union
{
  char __size[8];
  long int __align;
} pthread_rwlockattr_t;
typedef volatile int pthread_spinlock_t;
typedef union
{
  char __size[32];
  long int __align;
} pthread_barrier_t;
typedef union
{
  char __size[4];
  int __align;
} pthread_barrierattr_t;

extern long int random (void) ;
extern void srandom (unsigned int __seed) ;
extern char *initstate (unsigned int __seed, char *__statebuf,
   size_t __statelen) ;
extern char *setstate (char *__statebuf) ;
struct random_data
  {
    int32_t *fptr;
    int32_t *rptr;
    int32_t *state;
    int rand_type;
    int rand_deg;
    int rand_sep;
    int32_t *end_ptr;
  };
extern int random_r (struct random_data *restrict __buf,
       int32_t *restrict __result) ;
extern int srandom_r (unsigned int __seed, struct random_data *__buf)
     ;
extern int initstate_r (unsigned int __seed, char *restrict __statebuf,
   size_t __statelen,
   struct random_data *restrict __buf)
     ;
extern int setstate_r (char *restrict __statebuf,
         struct random_data *restrict __buf)
     ;
extern int rand (void) ;
extern void srand (unsigned int __seed) ;
extern int rand_r (unsigned int *__seed) ;
extern double drand48 (void) ;
extern double erand48 (unsigned short int __xsubi[3]) ;
extern long int lrand48 (void) ;
extern long int nrand48 (unsigned short int __xsubi[3])
     ;
extern long int mrand48 (void) ;
extern long int jrand48 (unsigned short int __xsubi[3])
     ;
extern void srand48 (long int __seedval) ;
extern unsigned short int *seed48 (unsigned short int __seed16v[3])
     ;
extern void lcong48 (unsigned short int __param[7]) ;
struct drand48_data
  {
    unsigned short int __x[3];
    unsigned short int __old_x[3];
    unsigned short int __c;
    unsigned short int __init;
    unsigned long long int __a;
  };
extern int drand48_r (struct drand48_data *restrict __buffer,
        double *restrict __result) ;
extern int erand48_r (unsigned short int __xsubi[3],
        struct drand48_data *restrict __buffer,
        double *restrict __result) ;
extern int lrand48_r (struct drand48_data *restrict __buffer,
        long int *restrict __result)
     ;
extern int nrand48_r (unsigned short int __xsubi[3],
        struct drand48_data *restrict __buffer,
        long int *restrict __result)
     ;
extern int mrand48_r (struct drand48_data *restrict __buffer,
        long int *restrict __result)
     ;
extern int jrand48_r (unsigned short int __xsubi[3],
        struct drand48_data *restrict __buffer,
        long int *restrict __result)
     ;
extern int srand48_r (long int __seedval, struct drand48_data *__buffer)
     ;
extern int seed48_r (unsigned short int __seed16v[3],
       struct drand48_data *__buffer) ;
extern int lcong48_r (unsigned short int __param[7],
        struct drand48_data *__buffer)
     ;
extern void *malloc (size_t __size)
     ;
extern void *calloc (size_t __nmemb, size_t __size)
     ;
extern void *realloc (void *__ptr, size_t __size)
     ;
extern void *reallocarray (void *__ptr, size_t __nmemb, size_t __size)
    
     ;
extern void free (void *__ptr) ;

extern void *alloca (size_t __size) ;

extern void *valloc (size_t __size)
     ;
extern int posix_memalign (void **__memptr, size_t __alignment, size_t __size)
     ;
extern void *aligned_alloc (size_t __alignment, size_t __size)
     ;
extern void abort (void) ;
extern int atexit (void (*__func) (void)) ;
extern int at_quick_exit (void (*__func) (void)) ;
extern int on_exit (void (*__func) (int __status, void *__arg), void *__arg)
     ;
extern void exit (int __status) ;
extern void quick_exit (int __status) ;
extern void _Exit (int __status) ;
extern char *getenv (const char *__name) ;
extern int putenv (char *__string) ;
extern int setenv (const char *__name, const char *__value, int __replace)
     ;
extern int unsetenv (const char *__name) ;
extern int clearenv (void) ;
extern char *mktemp (char *__template) ;
extern int mkstemp (char *__template) ;
extern int mkstemps (char *__template, int __suffixlen) ;
extern char *mkdtemp (char *__template) ;
extern int system (const char *__command) ;
extern char *realpath (const char *restrict __name,
         char *restrict __resolved) ;
typedef int (*__compar_fn_t) (const void *, const void *);
extern void *bsearch (const void *__key, const void *__base,
        size_t __nmemb, size_t __size, __compar_fn_t __compar)
     ;
extern void qsort (void *__base, size_t __nmemb, size_t __size,
     __compar_fn_t __compar) ;
extern int abs (int __x) ;
extern long int labs (long int __x) ;
 extern long long int llabs (long long int __x)
     ;
extern div_t div (int __numer, int __denom)
     ;
extern ldiv_t ldiv (long int __numer, long int __denom)
     ;
 extern lldiv_t lldiv (long long int __numer,
        long long int __denom)
     ;
extern char *ecvt (double __value, int __ndigit, int *restrict __decpt,
     int *restrict __sign) ;
extern char *fcvt (double __value, int __ndigit, int *restrict __decpt,
     int *restrict __sign) ;
extern char *gcvt (double __value, int __ndigit, char *__buf)
     ;
extern char *qecvt (long double __value, int __ndigit,
      int *restrict __decpt, int *restrict __sign)
     ;
extern char *qfcvt (long double __value, int __ndigit,
      int *restrict __decpt, int *restrict __sign)
     ;
extern char *qgcvt (long double __value, int __ndigit, char *__buf)
     ;
extern int ecvt_r (double __value, int __ndigit, int *restrict __decpt,
     int *restrict __sign, char *restrict __buf,
     size_t __len) ;
extern int fcvt_r (double __value, int __ndigit, int *restrict __decpt,
     int *restrict __sign, char *restrict __buf,
     size_t __len) ;
extern int qecvt_r (long double __value, int __ndigit,
      int *restrict __decpt, int *restrict __sign,
      char *restrict __buf, size_t __len)
     ;
extern int qfcvt_r (long double __value, int __ndigit,
      int *restrict __decpt, int *restrict __sign,
      char *restrict __buf, size_t __len)
     ;
extern int mblen (const char *__s, size_t __n) ;
extern int mbtowc (wchar_t *restrict __pwc,
     const char *restrict __s, size_t __n) ;
extern int wctomb (char *__s, wchar_t __wchar) ;
extern size_t mbstowcs (wchar_t *restrict __pwcs,
   const char *restrict __s, size_t __n) ;
extern size_t wcstombs (char *restrict __s,
   const wchar_t *restrict __pwcs, size_t __n)
     ;
extern int rpmatch (const char *__response) ;
extern int getsubopt (char **restrict __optionp,
        char *const *restrict __tokens,
        char **restrict __valuep)
     ;
extern int getloadavg (double __loadavg[], int __nelem)
     ;


extern void *memcpy (void *restrict __dest, const void *restrict __src,
       size_t __n) ;
extern void *memmove (void *__dest, const void *__src, size_t __n)
     ;
extern void *memccpy (void *restrict __dest, const void *restrict __src,
        int __c, size_t __n)
     ;
extern void *memset (void *__s, int __c, size_t __n) ;
extern int memcmp (const void *__s1, const void *__s2, size_t __n)
     ;
extern void *memchr (const void *__s, int __c, size_t __n)
      ;
extern char *strcpy (char *restrict __dest, const char *restrict __src)
     ;
extern char *strncpy (char *restrict __dest,
        const char *restrict __src, size_t __n)
     ;
extern char *strcat (char *restrict __dest, const char *restrict __src)
     ;
extern char *strncat (char *restrict __dest, const char *restrict __src,
        size_t __n) ;
extern int strcmp (const char *__s1, const char *__s2)
     ;
extern int strncmp (const char *__s1, const char *__s2, size_t __n)
     ;
extern int strcoll (const char *__s1, const char *__s2)
     ;
extern size_t strxfrm (char *restrict __dest,
         const char *restrict __src, size_t __n)
     ;
struct __locale_struct
{
  struct __locale_data *__locales[13];
  const unsigned short int *__ctype_b;
  const int *__ctype_tolower;
  const int *__ctype_toupper;
  const char *__names[13];
};
typedef struct __locale_struct *__locale_t;
typedef __locale_t locale_t;
extern int strcoll_l (const char *__s1, const char *__s2, locale_t __l)
     ;
extern size_t strxfrm_l (char *__dest, const char *__src, size_t __n,
    locale_t __l) ;
extern char *strdup (const char *__s)
     ;
extern char *strndup (const char *__string, size_t __n)
     ;
extern char *strchr (const char *__s, int __c)
     ;
extern char *strrchr (const char *__s, int __c)
     ;
extern size_t strcspn (const char *__s, const char *__reject)
     ;
extern size_t strspn (const char *__s, const char *__accept)
     ;
extern char *strpbrk (const char *__s, const char *__accept)
     ;
extern char *strstr (const char *__haystack, const char *__needle)
     ;
extern char *strtok (char *restrict __s, const char *restrict __delim)
     ;
extern char *__strtok_r (char *restrict __s,
    const char *restrict __delim,
    char **restrict __save_ptr)
     ;
extern char *strtok_r (char *restrict __s, const char *restrict __delim,
         char **restrict __save_ptr)
     ;
extern size_t strlen (const char *__s)
     ;
extern size_t strnlen (const char *__string, size_t __maxlen)
     ;
extern char *strerror (int __errnum) ;
extern int __xpg_strerror_r (int __errnum, char *__buf, size_t __buflen)
     ;
extern char *strerror_l (int __errnum, locale_t __l) ;

extern int bcmp (const void *__s1, const void *__s2, size_t __n)
     ;
extern void bcopy (const void *__src, void *__dest, size_t __n)
  ;
extern void bzero (void *__s, size_t __n) ;
extern char *index (const char *__s, int __c)
     ;
extern char *rindex (const char *__s, int __c)
     ;
extern int ffs (int __i) ;
extern int ffsl (long int __l) ;
 extern int ffsll (long long int __ll)
     ;
extern int strcasecmp (const char *__s1, const char *__s2)
     ;
extern int strncasecmp (const char *__s1, const char *__s2, size_t __n)
     ;
extern int strcasecmp_l (const char *__s1, const char *__s2, locale_t __loc)
     ;
extern int strncasecmp_l (const char *__s1, const char *__s2,
     size_t __n, locale_t __loc)
     ;

extern void explicit_bzero (void *__s, size_t __n) ;
extern char *strsep (char **restrict __stringp,
       const char *restrict __delim)
     ;
extern char *strsignal (int __sig) ;
extern char *__stpcpy (char *restrict __dest, const char *restrict __src)
     ;
extern char *stpcpy (char *restrict __dest, const char *restrict __src)
     ;
extern char *__stpncpy (char *restrict __dest,
   const char *restrict __src, size_t __n)
     ;
extern char *stpncpy (char *restrict __dest,
        const char *restrict __src, size_t __n)
     ;


typedef __useconds_t useconds_t;
typedef __intptr_t intptr_t;
typedef __socklen_t socklen_t;
extern int access (const char *__name, int __type) ;
extern int faccessat (int __fd, const char *__file, int __type, int __flag)
     ;
extern __off_t lseek (int __fd, __off_t __offset, int __whence) ;
extern int close (int __fd);
extern ssize_t read (int __fd, void *__buf, size_t __nbytes) ;
extern ssize_t write (int __fd, const void *__buf, size_t __n) ;
extern ssize_t pread (int __fd, void *__buf, size_t __nbytes,
        __off_t __offset) ;
extern ssize_t pwrite (int __fd, const void *__buf, size_t __n,
         __off_t __offset) ;
extern int pipe (int __pipedes[2]) ;
extern unsigned int alarm (unsigned int __seconds) ;
extern unsigned int sleep (unsigned int __seconds);
extern __useconds_t ualarm (__useconds_t __value, __useconds_t __interval)
     ;
extern int usleep (__useconds_t __useconds);
extern int pause (void);
extern int chown (const char *__file, __uid_t __owner, __gid_t __group)
     ;
extern int fchown (int __fd, __uid_t __owner, __gid_t __group) ;
extern int lchown (const char *__file, __uid_t __owner, __gid_t __group)
     ;
extern int fchownat (int __fd, const char *__file, __uid_t __owner,
       __gid_t __group, int __flag)
     ;
extern int chdir (const char *__path) ;
extern int fchdir (int __fd) ;
extern char *getcwd (char *__buf, size_t __size) ;
extern char *getwd (char *__buf)
     ;
extern int dup (int __fd) ;
extern int dup2 (int __fd, int __fd2) ;
extern char **__environ;
extern int execve (const char *__path, char *const __argv[],
     char *const __envp[]) ;
extern int fexecve (int __fd, char *const __argv[], char *const __envp[])
     ;
extern int execv (const char *__path, char *const __argv[])
     ;
extern int execle (const char *__path, const char *__arg, ...)
     ;
extern int execl (const char *__path, const char *__arg, ...)
     ;
extern int execvp (const char *__file, char *const __argv[])
     ;
extern int execlp (const char *__file, const char *__arg, ...)
     ;
extern int nice (int __inc) ;
extern void _exit (int __status) ;
enum
  {
    _PC_LINK_MAX,
    _PC_MAX_CANON,
    _PC_MAX_INPUT,
    _PC_NAME_MAX,
    _PC_PATH_MAX,
    _PC_PIPE_BUF,
    _PC_CHOWN_RESTRICTED,
    _PC_NO_TRUNC,
    _PC_VDISABLE,
    _PC_SYNC_IO,
    _PC_ASYNC_IO,
    _PC_PRIO_IO,
    _PC_SOCK_MAXBUF,
    _PC_FILESIZEBITS,
    _PC_REC_INCR_XFER_SIZE,
    _PC_REC_MAX_XFER_SIZE,
    _PC_REC_MIN_XFER_SIZE,
    _PC_REC_XFER_ALIGN,
    _PC_ALLOC_SIZE_MIN,
    _PC_SYMLINK_MAX,
    _PC_2_SYMLINKS
  };
enum
  {
    _SC_ARG_MAX,
    _SC_CHILD_MAX,
    _SC_CLK_TCK,
    _SC_NGROUPS_MAX,
    _SC_OPEN_MAX,
    _SC_STREAM_MAX,
    _SC_TZNAME_MAX,
    _SC_JOB_CONTROL,
    _SC_SAVED_IDS,
    _SC_REALTIME_SIGNALS,
    _SC_PRIORITY_SCHEDULING,
    _SC_TIMERS,
    _SC_ASYNCHRONOUS_IO,
    _SC_PRIORITIZED_IO,
    _SC_SYNCHRONIZED_IO,
    _SC_FSYNC,
    _SC_MAPPED_FILES,
    _SC_MEMLOCK,
    _SC_MEMLOCK_RANGE,
    _SC_MEMORY_PROTECTION,
    _SC_MESSAGE_PASSING,
    _SC_SEMAPHORES,
    _SC_SHARED_MEMORY_OBJECTS,
    _SC_AIO_LISTIO_MAX,
    _SC_AIO_MAX,
    _SC_AIO_PRIO_DELTA_MAX,
    _SC_DELAYTIMER_MAX,
    _SC_MQ_OPEN_MAX,
    _SC_MQ_PRIO_MAX,
    _SC_VERSION,
    _SC_PAGESIZE,
    _SC_RTSIG_MAX,
    _SC_SEM_NSEMS_MAX,
    _SC_SEM_VALUE_MAX,
    _SC_SIGQUEUE_MAX,
    _SC_TIMER_MAX,
    _SC_BC_BASE_MAX,
    _SC_BC_DIM_MAX,
    _SC_BC_SCALE_MAX,
    _SC_BC_STRING_MAX,
    _SC_COLL_WEIGHTS_MAX,
    _SC_EQUIV_CLASS_MAX,
    _SC_EXPR_NEST_MAX,
    _SC_LINE_MAX,
    _SC_RE_DUP_MAX,
    _SC_CHARCLASS_NAME_MAX,
    _SC_2_VERSION,
    _SC_2_C_BIND,
    _SC_2_C_DEV,
    _SC_2_FORT_DEV,
    _SC_2_FORT_RUN,
    _SC_2_SW_DEV,
    _SC_2_LOCALEDEF,
    _SC_PII,
    _SC_PII_XTI,
    _SC_PII_SOCKET,
    _SC_PII_INTERNET,
    _SC_PII_OSI,
    _SC_POLL,
    _SC_SELECT,
    _SC_UIO_MAXIOV,
    _SC_IOV_MAX = _SC_UIO_MAXIOV,
    _SC_PII_INTERNET_STREAM,
    _SC_PII_INTERNET_DGRAM,
    _SC_PII_OSI_COTS,
    _SC_PII_OSI_CLTS,
    _SC_PII_OSI_M,
    _SC_T_IOV_MAX,
    _SC_THREADS,
    _SC_THREAD_SAFE_FUNCTIONS,
    _SC_GETGR_R_SIZE_MAX,
    _SC_GETPW_R_SIZE_MAX,
    _SC_LOGIN_NAME_MAX,
    _SC_TTY_NAME_MAX,
    _SC_THREAD_DESTRUCTOR_ITERATIONS,
    _SC_THREAD_KEYS_MAX,
    _SC_THREAD_STACK_MIN,
    _SC_THREAD_THREADS_MAX,
    _SC_THREAD_ATTR_STACKADDR,
    _SC_THREAD_ATTR_STACKSIZE,
    _SC_THREAD_PRIORITY_SCHEDULING,
    _SC_THREAD_PRIO_INHERIT,
    _SC_THREAD_PRIO_PROTECT,
    _SC_THREAD_PROCESS_SHARED,
    _SC_NPROCESSORS_CONF,
    _SC_NPROCESSORS_ONLN,
    _SC_PHYS_PAGES,
    _SC_AVPHYS_PAGES,
    _SC_ATEXIT_MAX,
    _SC_PASS_MAX,
    _SC_XOPEN_VERSION,
    _SC_XOPEN_XCU_VERSION,
    _SC_XOPEN_UNIX,
    _SC_XOPEN_CRYPT,
    _SC_XOPEN_ENH_I18N,
    _SC_XOPEN_SHM,
    _SC_2_CHAR_TERM,
    _SC_2_C_VERSION,
    _SC_2_UPE,
    _SC_XOPEN_XPG2,
    _SC_XOPEN_XPG3,
    _SC_XOPEN_XPG4,
    _SC_CHAR_BIT,
    _SC_CHAR_MAX,
    _SC_CHAR_MIN,
    _SC_INT_MAX,
    _SC_INT_MIN,
    _SC_LONG_BIT,
    _SC_WORD_BIT,
    _SC_MB_LEN_MAX,
    _SC_NZERO,
    _SC_SSIZE_MAX,
    _SC_SCHAR_MAX,
    _SC_SCHAR_MIN,
    _SC_SHRT_MAX,
    _SC_SHRT_MIN,
    _SC_UCHAR_MAX,
    _SC_UINT_MAX,
    _SC_ULONG_MAX,
    _SC_USHRT_MAX,
    _SC_NL_ARGMAX,
    _SC_NL_LANGMAX,
    _SC_NL_MSGMAX,
    _SC_NL_NMAX,
    _SC_NL_SETMAX,
    _SC_NL_TEXTMAX,
    _SC_XBS5_ILP32_OFF32,
    _SC_XBS5_ILP32_OFFBIG,
    _SC_XBS5_LP64_OFF64,
    _SC_XBS5_LPBIG_OFFBIG,
    _SC_XOPEN_LEGACY,
    _SC_XOPEN_REALTIME,
    _SC_XOPEN_REALTIME_THREADS,
    _SC_ADVISORY_INFO,
    _SC_BARRIERS,
    _SC_BASE,
    _SC_C_LANG_SUPPORT,
    _SC_C_LANG_SUPPORT_R,
    _SC_CLOCK_SELECTION,
    _SC_CPUTIME,
    _SC_THREAD_CPUTIME,
    _SC_DEVICE_IO,
    _SC_DEVICE_SPECIFIC,
    _SC_DEVICE_SPECIFIC_R,
    _SC_FD_MGMT,
    _SC_FIFO,
    _SC_PIPE,
    _SC_FILE_ATTRIBUTES,
    _SC_FILE_LOCKING,
    _SC_FILE_SYSTEM,
    _SC_MONOTONIC_CLOCK,
    _SC_MULTI_PROCESS,
    _SC_SINGLE_PROCESS,
    _SC_NETWORKING,
    _SC_READER_WRITER_LOCKS,
    _SC_SPIN_LOCKS,
    _SC_REGEXP,
    _SC_REGEX_VERSION,
    _SC_SHELL,
    _SC_SIGNALS,
    _SC_SPAWN,
    _SC_SPORADIC_SERVER,
    _SC_THREAD_SPORADIC_SERVER,
    _SC_SYSTEM_DATABASE,
    _SC_SYSTEM_DATABASE_R,
    _SC_TIMEOUTS,
    _SC_TYPED_MEMORY_OBJECTS,
    _SC_USER_GROUPS,
    _SC_USER_GROUPS_R,
    _SC_2_PBS,
    _SC_2_PBS_ACCOUNTING,
    _SC_2_PBS_LOCATE,
    _SC_2_PBS_MESSAGE,
    _SC_2_PBS_TRACK,
    _SC_SYMLOOP_MAX,
    _SC_STREAMS,
    _SC_2_PBS_CHECKPOINT,
    _SC_V6_ILP32_OFF32,
    _SC_V6_ILP32_OFFBIG,
    _SC_V6_LP64_OFF64,
    _SC_V6_LPBIG_OFFBIG,
    _SC_HOST_NAME_MAX,
    _SC_TRACE,
    _SC_TRACE_EVENT_FILTER,
    _SC_TRACE_INHERIT,
    _SC_TRACE_LOG,
    _SC_LEVEL1_ICACHE_SIZE,
    _SC_LEVEL1_ICACHE_ASSOC,
    _SC_LEVEL1_ICACHE_LINESIZE,
    _SC_LEVEL1_DCACHE_SIZE,
    _SC_LEVEL1_DCACHE_ASSOC,
    _SC_LEVEL1_DCACHE_LINESIZE,
    _SC_LEVEL2_CACHE_SIZE,
    _SC_LEVEL2_CACHE_ASSOC,
    _SC_LEVEL2_CACHE_LINESIZE,
    _SC_LEVEL3_CACHE_SIZE,
    _SC_LEVEL3_CACHE_ASSOC,
    _SC_LEVEL3_CACHE_LINESIZE,
    _SC_LEVEL4_CACHE_SIZE,
    _SC_LEVEL4_CACHE_ASSOC,
    _SC_LEVEL4_CACHE_LINESIZE,
    _SC_IPV6 = _SC_LEVEL1_ICACHE_SIZE + 50,
    _SC_RAW_SOCKETS,
    _SC_V7_ILP32_OFF32,
    _SC_V7_ILP32_OFFBIG,
    _SC_V7_LP64_OFF64,
    _SC_V7_LPBIG_OFFBIG,
    _SC_SS_REPL_MAX,
    _SC_TRACE_EVENT_NAME_MAX,
    _SC_TRACE_NAME_MAX,
    _SC_TRACE_SYS_MAX,
    _SC_TRACE_USER_EVENT_MAX,
    _SC_XOPEN_STREAMS,
    _SC_THREAD_ROBUST_PRIO_INHERIT,
    _SC_THREAD_ROBUST_PRIO_PROTECT
  };
enum
  {
    _CS_PATH,
    _CS_V6_WIDTH_RESTRICTED_ENVS,
    _CS_GNU_LIBC_VERSION,
    _CS_GNU_LIBPTHREAD_VERSION,
    _CS_V5_WIDTH_RESTRICTED_ENVS,
    _CS_V7_WIDTH_RESTRICTED_ENVS,
    _CS_LFS_CFLAGS = 1000,
    _CS_LFS_LDFLAGS,
    _CS_LFS_LIBS,
    _CS_LFS_LINTFLAGS,
    _CS_LFS64_CFLAGS,
    _CS_LFS64_LDFLAGS,
    _CS_LFS64_LIBS,
    _CS_LFS64_LINTFLAGS,
    _CS_XBS5_ILP32_OFF32_CFLAGS = 1100,
    _CS_XBS5_ILP32_OFF32_LDFLAGS,
    _CS_XBS5_ILP32_OFF32_LIBS,
    _CS_XBS5_ILP32_OFF32_LINTFLAGS,
    _CS_XBS5_ILP32_OFFBIG_CFLAGS,
    _CS_XBS5_ILP32_OFFBIG_LDFLAGS,
    _CS_XBS5_ILP32_OFFBIG_LIBS,
    _CS_XBS5_ILP32_OFFBIG_LINTFLAGS,
    _CS_XBS5_LP64_OFF64_CFLAGS,
    _CS_XBS5_LP64_OFF64_LDFLAGS,
    _CS_XBS5_LP64_OFF64_LIBS,
    _CS_XBS5_LP64_OFF64_LINTFLAGS,
    _CS_XBS5_LPBIG_OFFBIG_CFLAGS,
    _CS_XBS5_LPBIG_OFFBIG_LDFLAGS,
    _CS_XBS5_LPBIG_OFFBIG_LIBS,
    _CS_XBS5_LPBIG_OFFBIG_LINTFLAGS,
    _CS_POSIX_V6_ILP32_OFF32_CFLAGS,
    _CS_POSIX_V6_ILP32_OFF32_LDFLAGS,
    _CS_POSIX_V6_ILP32_OFF32_LIBS,
    _CS_POSIX_V6_ILP32_OFF32_LINTFLAGS,
    _CS_POSIX_V6_ILP32_OFFBIG_CFLAGS,
    _CS_POSIX_V6_ILP32_OFFBIG_LDFLAGS,
    _CS_POSIX_V6_ILP32_OFFBIG_LIBS,
    _CS_POSIX_V6_ILP32_OFFBIG_LINTFLAGS,
    _CS_POSIX_V6_LP64_OFF64_CFLAGS,
    _CS_POSIX_V6_LP64_OFF64_LDFLAGS,
    _CS_POSIX_V6_LP64_OFF64_LIBS,
    _CS_POSIX_V6_LP64_OFF64_LINTFLAGS,
    _CS_POSIX_V6_LPBIG_OFFBIG_CFLAGS,
    _CS_POSIX_V6_LPBIG_OFFBIG_LDFLAGS,
    _CS_POSIX_V6_LPBIG_OFFBIG_LIBS,
    _CS_POSIX_V6_LPBIG_OFFBIG_LINTFLAGS,
    _CS_POSIX_V7_ILP32_OFF32_CFLAGS,
    _CS_POSIX_V7_ILP32_OFF32_LDFLAGS,
    _CS_POSIX_V7_ILP32_OFF32_LIBS,
    _CS_POSIX_V7_ILP32_OFF32_LINTFLAGS,
    _CS_POSIX_V7_ILP32_OFFBIG_CFLAGS,
    _CS_POSIX_V7_ILP32_OFFBIG_LDFLAGS,
    _CS_POSIX_V7_ILP32_OFFBIG_LIBS,
    _CS_POSIX_V7_ILP32_OFFBIG_LINTFLAGS,
    _CS_POSIX_V7_LP64_OFF64_CFLAGS,
    _CS_POSIX_V7_LP64_OFF64_LDFLAGS,
    _CS_POSIX_V7_LP64_OFF64_LIBS,
    _CS_POSIX_V7_LP64_OFF64_LINTFLAGS,
    _CS_POSIX_V7_LPBIG_OFFBIG_CFLAGS,
    _CS_POSIX_V7_LPBIG_OFFBIG_LDFLAGS,
    _CS_POSIX_V7_LPBIG_OFFBIG_LIBS,
    _CS_POSIX_V7_LPBIG_OFFBIG_LINTFLAGS,
    _CS_V6_ENV,
    _CS_V7_ENV
  };
extern long int pathconf (const char *__path, int __name)
     ;
extern long int fpathconf (int __fd, int __name) ;
extern long int sysconf (int __name) ;
extern size_t confstr (int __name, char *__buf, size_t __len) ;
extern __pid_t getpid (void) ;
extern __pid_t getppid (void) ;
extern __pid_t getpgrp (void) ;
extern __pid_t __getpgid (__pid_t __pid) ;
extern __pid_t getpgid (__pid_t __pid) ;
extern int setpgid (__pid_t __pid, __pid_t __pgid) ;
extern int setpgrp (void) ;
extern __pid_t setsid (void) ;
extern __pid_t getsid (__pid_t __pid) ;
extern __uid_t getuid (void) ;
extern __uid_t geteuid (void) ;
extern __gid_t getgid (void) ;
extern __gid_t getegid (void) ;
extern int getgroups (int __size, __gid_t __list[]) ;
extern int setuid (__uid_t __uid) ;
extern int setreuid (__uid_t __ruid, __uid_t __euid) ;
extern int seteuid (__uid_t __uid) ;
extern int setgid (__gid_t __gid) ;
extern int setregid (__gid_t __rgid, __gid_t __egid) ;
extern int setegid (__gid_t __gid) ;
extern __pid_t fork (void) ;
extern __pid_t vfork (void) ;
extern char *ttyname (int __fd) ;
extern int ttyname_r (int __fd, char *__buf, size_t __buflen)
     ;
extern int isatty (int __fd) ;
extern int ttyslot (void) ;
extern int link (const char *__from, const char *__to)
     ;
extern int linkat (int __fromfd, const char *__from, int __tofd,
     const char *__to, int __flags)
     ;
extern int symlink (const char *__from, const char *__to)
     ;
extern ssize_t readlink (const char *restrict __path,
    char *restrict __buf, size_t __len)
     ;
extern int symlinkat (const char *__from, int __tofd,
        const char *__to) ;
extern ssize_t readlinkat (int __fd, const char *restrict __path,
      char *restrict __buf, size_t __len)
     ;
extern int unlink (const char *__name) ;
extern int unlinkat (int __fd, const char *__name, int __flag)
     ;
extern int rmdir (const char *__path) ;
extern __pid_t tcgetpgrp (int __fd) ;
extern int tcsetpgrp (int __fd, __pid_t __pgrp_id) ;
extern char *getlogin (void);
extern int getlogin_r (char *__name, size_t __name_len) ;
extern int setlogin (const char *__name) ;

extern char *optarg;
extern int optind;
extern int opterr;
extern int optopt;
extern int getopt (int ___argc, char *const *___argv, const char *__shortopts)
       ;



extern int gethostname (char *__name, size_t __len) ;
extern int sethostname (const char *__name, size_t __len)
     ;
extern int sethostid (long int __id) ;
extern int getdomainname (char *__name, size_t __len)
     ;
extern int setdomainname (const char *__name, size_t __len)
     ;
extern int vhangup (void) ;
extern int revoke (const char *__file) ;
extern int profil (unsigned short int *__sample_buffer, size_t __size,
     size_t __offset, unsigned int __scale)
     ;
extern int acct (const char *__name) ;
extern char *getusershell (void) ;
extern void endusershell (void) ;
extern void setusershell (void) ;
extern int daemon (int __nochdir, int __noclose) ;
extern int chroot (const char *__path) ;
extern char *getpass (const char *__prompt) ;
extern int fsync (int __fd);
extern long int gethostid (void);
extern void sync (void) ;
extern int getpagesize (void) ;
extern int getdtablesize (void) ;
extern int truncate (const char *__file, __off_t __length)
     ;
extern int ftruncate (int __fd, __off_t __length) ;
extern int brk (void *__addr) ;
extern void *sbrk (intptr_t __delta) ;
extern long int syscall (long int __sysno, ...) ;
extern int lockf (int __fd, int __cmd, __off_t __len) ;
extern int fdatasync (int __fildes);
extern char *crypt (const char *__key, const char *__salt)
     ;
int getentropy (void *__buffer, size_t __length) ;

int align(int val, int bound);
 void err(const char *fmt, ...);
typedef struct string string_t;
struct string {
    size_t length;
    size_t capacity;
    char *data;
};
void string_init(string_t *self);
void string_free(string_t *self);
void string_clear(string_t *self);
void string_reserve(string_t *self, size_t new_capacity);
void string_push(string_t *self, char c);
void string_vprintf(string_t *self, const char *fmt, va_list ap);
void string_printf(string_t *self, const char *fmt, ...);
enum {
    TK_EOF,
    TK_AUTO,
    TK_BREAK,
    TK_CASE,
    TK_CHAR,
    TK_CONST,
    TK_CONTINUE,
    TK_DEFAULT,
    TK_DO,
    TK_DOUBLE,
    TK_ELSE,
    TK_ENUM,
    TK_EXTERN,
    TK_FLOAT,
    TK_FOR,
    TK_GOTO,
    TK_IF,
    TK_INLINE,
    TK_INT,
    TK_LONG,
    TK_REGISTER,
    TK_RESTRICT,
    TK_RETURN,
    TK_SHORT,
    TK_SIGNED,
    TK_SIZEOF,
    TK_STATIC,
    TK_STRUCT,
    TK_SWITCH,
    TK_TYPEDEF,
    TK_UNION,
    TK_UNSIGNED,
    TK_VOID,
    TK_VOLATILE,
    TK_WHILE,
    TK_BOOL,
    TK_COMPLEX,
    TK_IMAGINARY,
    TK_IDENTIFIER,
    TK_CONSTANT,
    TK_STR_LIT,
    TK_LSQ,
    TK_RSQ,
    TK_LPAREN,
    TK_RPAREN,
    TK_LCURLY,
    TK_RCURLY,
    TK_DOT,
    TK_ARROW,
    TK_INCR,
    TK_DECR,
    TK_AND,
    TK_MUL,
    TK_ADD,
    TK_SUB,
    TK_NOT,
    TK_LNOT,
    TK_DIV,
    TK_MOD,
    TK_LSH,
    TK_RSH,
    TK_LT,
    TK_GT,
    TK_LE,
    TK_GE,
    TK_EQ,
    TK_NE,
    TK_XOR,
    TK_OR,
    TK_LAND,
    TK_LOR,
    TK_COND,
    TK_COLON,
    TK_SEMICOLON,
    TK_ELLIPSIS,
    TK_AS,
    TK_MUL_AS,
    TK_DIV_AS,
    TK_MOD_AS,
    TK_ADD_AS,
    TK_SUB_AS,
    TK_LSH_AS,
    TK_RSH_AS,
    TK_AND_AS,
    TK_XOR_AS,
    TK_OR_AS,
    TK_COMMA,
    TK_HASH,
    TK_PASTE,
    TK_ERROR,
};
typedef struct tk tk_t;
typedef unsigned long long val_t;
struct tk {
    int type;
    size_t line, col;
    string_t spelling;
    val_t val;
    string_t str;
};
static inline const char *tk_str(tk_t *tk)
{
    return tk->spelling.data;
}
typedef struct lexer lexer_t;
struct lexer {
    FILE *fp;
    char buf[8192], *cur, *end;
    size_t line, col;
    tk_t tk_buf[2];
    int tk_pos, tk_cnt;
    tk_t *tk;
};
void lex_init(lexer_t *self, FILE *fp);
void lex_free(lexer_t *self);
tk_t *lex_tok(lexer_t *self, int i);
void lex_adv(lexer_t *self);
typedef struct ty ty_t;
typedef struct memb memb_t;
struct memb {
    memb_t *next;
    ty_t *ty;
    char *name;
    int offset;
};
memb_t *make_memb(ty_t *ty, char *name);
memb_t *pack_struct(memb_t *members, int *out_align, int *out_size);
memb_t *pack_union(memb_t *members, int *out_align, int *out_size);
enum {
    TAG_STRUCT,
    TAG_UNION,
    TAG_ENUM,
};
typedef struct tag tag_t;
struct tag {
    tag_t *next;
    int kind;
    char *name;
    _Bool defined;
    memb_t *members;
    int align;
    int size;
};
enum {
    TY_VOID,
    TY_CHAR,
    TY_SCHAR,
    TY_UCHAR,
    TY_SHORT,
    TY_USHORT,
    TY_INT,
    TY_UINT,
    TY_LONG,
    TY_ULONG,
    TY_LLONG,
    TY_ULLONG,
    TY_FLOAT,
    TY_DOUBLE,
    TY_LDOUBLE,
    TY_BOOL,
    TY_TAG,
    TY_POINTER,
    TY_ARRAY,
    TY_FUNCTION,
};
typedef struct scope scope_t;
struct ty {
    ty_t *next;
    int kind;
    union {
        tag_t *tag;
        struct {
            ty_t *base_ty;
        } pointer;
        struct {
            ty_t *elem_ty;
            int cnt;
        } array;
        struct {
            ty_t *ret_ty;
            ty_t *param_tys;
            _Bool var;
            scope_t *scope;
        } function;
    };
};
ty_t *make_ty(int kind);
ty_t *make_ty_tag(tag_t *tag);
ty_t *make_pointer(ty_t *base_ty);
ty_t *make_array(ty_t *elem_ty, int cnt);
ty_t *make_param(ty_t *ty);
ty_t *make_function(ty_t *ret_ty, ty_t *param_tys, _Bool var);
ty_t *clone_ty(ty_t *ty);
void free_ty(ty_t *ty);
void print_ty(ty_t *ty);
int ty_align(ty_t *ty);
int ty_size(ty_t *ty);
enum {
    SYM_TYPEDEF,
    SYM_EXTERN,
    SYM_STATIC,
    SYM_LOCAL,
    SYM_ENUM_CONST,
};
typedef struct sym sym_t;
struct sym {
    sym_t *next;
    int kind;
    ty_t *ty;
    char *name;
    _Bool had_def;
    int offset;
    val_t val;
};
struct scope {
    scope_t *parent;
    sym_t *syms;
    tag_t *tags;
};
typedef struct sema sema_t;
struct sema {
    scope_t *scope;
    int offset;
};
void sema_init(sema_t *self);
void sema_free(sema_t *self);
void sema_enter(sema_t *self);
void sema_exit(sema_t *self);
void sema_push(sema_t *self, scope_t *scope);
scope_t *sema_pop(sema_t *self);
sym_t *sema_declare(sema_t *self, int sc, ty_t *ty, char *name);
sym_t *sema_declare_enum_const(sema_t *self, char *name, val_t val);
sym_t *sema_lookup(sema_t *self, const char *name);
ty_t *sema_findtypedef(sema_t *self, const char *name);
tag_t *sema_forward_declare_tag(sema_t *self, int kind, const char *name);
tag_t *sema_define_tag(sema_t *self, int kind, const char *name);
enum {
    EXPR_GLOBAL,
    EXPR_LOCAL,
    EXPR_CONST,
    EXPR_STR_LIT,
    EXPR_MEMB,
    EXPR_CALL,
    EXPR_REF,
    EXPR_DREF,
    EXPR_POS,
    EXPR_NEG,
    EXPR_NOT,
    EXPR_MUL,
    EXPR_DIV,
    EXPR_MOD,
    EXPR_ADD,
    EXPR_SUB,
    EXPR_LSH,
    EXPR_RSH,
    EXPR_AND,
    EXPR_XOR,
    EXPR_OR,
    EXPR_LT,
    EXPR_GT,
    EXPR_LE,
    EXPR_GE,
    EXPR_EQ,
    EXPR_NE,
    EXPR_LNOT,
    EXPR_LAND,
    EXPR_LOR,
    EXPR_AS,
    EXPR_COND,
    EXPR_SEQ,
    EXPR_INIT,
};
typedef struct expr expr_t;
struct expr {
    expr_t *next;
    int kind;
    ty_t *ty;
    int offset;
    char *str;
    val_t val;
    expr_t *arg1, *arg2, *arg3;
};
expr_t *make_sym(sema_t *self, const char *name);
expr_t *make_const(ty_t *ty, val_t val);
expr_t *make_str_lit(const char *str);
expr_t *make_unary(int kind, expr_t *arg1);
expr_t *make_memb_expr(expr_t *arg1, const char *name);
expr_t *make_binary(int kind, expr_t *arg1, expr_t *arg2);
expr_t *make_trinary(int kind, expr_t *arg1, expr_t *arg2, expr_t *arg3);
enum {
    STMT_LABEL,
    STMT_CASE,
    STMT_DEFAULT,
    STMT_IF,
    STMT_SWITCH,
    STMT_WHILE,
    STMT_DO,
    STMT_FOR,
    STMT_GOTO,
    STMT_CONTINUE,
    STMT_BREAK,
    STMT_RETURN,
    STMT_INIT,
    STMT_EVAL,
};
typedef struct stmt stmt_t;
struct stmt {
    stmt_t *next;
    int kind;
    const char *label;
    int case_val;
    struct {
        ty_t *ty;
        int offset;
    } init;
    expr_t *arg1, *arg2, *arg3;
    stmt_t *body1, *body2;
};
stmt_t *make_stmt(int kind);
typedef struct gen gen_t;
struct gen {
    int offset;
    int label_cnt;
    int str_lit_cnt;
    string_t code;
    string_t data;
};
void gen_init(gen_t *self);
void gen_free(gen_t *self);
char *gen_str_lit(gen_t *self, tk_t *tk);
void gen_func(gen_t *self, sym_t *sym, int offset, stmt_t *body);
typedef struct cc3 cc3_t;
struct cc3 {
    lexer_t lexer;
    sema_t sema;
    gen_t gen;
};
void parse(cc3_t *self);
memb_t *make_memb(ty_t *ty, char *name)
{
    memb_t *memb = calloc(1, sizeof *memb);
    if (!memb) abort();
    memb->ty = ty;
    memb->name = name;
    return memb;
}
memb_t *pack_struct(memb_t *members, int *out_align, int *out_size)
{
    for (memb_t *memb = members; memb; memb = memb->next) {
        int memb_align = ty_align(memb->ty);
        if (memb_align > *out_align)
            *out_align = memb_align;
        *out_size = align(*out_size, memb_align);
        memb->offset = *out_size;
        *out_size += ty_size(memb->ty);
    }
    *out_size = align(*out_size, *out_align);
    return members;
}
memb_t *pack_union(memb_t *members, int *out_align, int *out_size)
{
    for (memb_t *memb = members; memb; memb = memb->next) {
        int memb_align = ty_align(memb->ty);
        int memb_size = ty_size(memb->ty);
        if (memb_align > *out_align)
            *out_align = memb_align;
        if (memb_size > *out_size)
            *out_size = memb_size;
    }
    return members;
}
ty_t *make_ty(int kind)
{
    ty_t *ty = calloc(1, sizeof *ty);
    if (!ty) abort();
    ty->kind = kind;
    return ty;
}
ty_t *make_ty_tag(tag_t *tag)
{
    ty_t *ty = make_ty(TY_TAG);
    ty->tag = tag;
    return ty;
}
ty_t *make_pointer(ty_t *base_ty)
{
    ty_t *ty = make_ty(TY_POINTER);
    ty->pointer.base_ty = base_ty;
    return ty;
}
ty_t *make_array(ty_t *elem_ty, int cnt)
{
    ty_t *ty = make_ty(TY_ARRAY);
    ty->array.elem_ty = elem_ty;
    ty->array.cnt = cnt;
    return ty;
}
ty_t *make_param(ty_t *ty)
{
    switch (ty->kind) {
    case TY_VOID:
        err("Parameter type cannot be void");
    case TY_ARRAY: {
        ty_t *elem_ty = ty->array.elem_ty;
        free(ty);
        return make_pointer(elem_ty);
    }
    case TY_FUNCTION:
        return make_pointer(ty);
    default:
        return ty;
    }
}
ty_t *make_function(ty_t *ret_ty, ty_t *param_tys, _Bool var)
{
    ty_t *ty = make_ty(TY_FUNCTION);
    if (ret_ty->kind == TY_ARRAY)
        err("Function returning array");
    if (ret_ty->kind == TY_FUNCTION)
        err("Function returning function");
    ty->function.ret_ty = ret_ty;
    ty->function.param_tys = param_tys;
    ty->function.var = var;
    return ty;
}
ty_t *clone_ty(ty_t *ty)
{
    switch (ty->kind) {
    case TY_VOID:
    case TY_CHAR:
    case TY_SCHAR:
    case TY_UCHAR:
    case TY_SHORT:
    case TY_USHORT:
    case TY_INT:
    case TY_UINT:
    case TY_LONG:
    case TY_ULONG:
    case TY_LLONG:
    case TY_ULLONG:
    case TY_FLOAT:
    case TY_DOUBLE:
    case TY_LDOUBLE:
    case TY_BOOL:
        return make_ty(ty->kind);
    case TY_TAG:
        return make_ty_tag(ty->tag);
    case TY_POINTER:
        return make_pointer(clone_ty(ty->pointer.base_ty));
    case TY_ARRAY:
        return make_array(clone_ty(ty->array.elem_ty), ty->array.cnt);
    case TY_FUNCTION:
    {
        ty_t *param_tys = ((void *)0), **tail = &param_tys;
        for (ty_t *param_ty = ty->function.param_tys; param_ty; param_ty = param_ty->next) {
            *tail = clone_ty(param_ty);
            tail = &(*tail)->next;
        }
        return make_function(clone_ty(ty->function.ret_ty), param_tys, ty->function.var);
    }
    default:
        ((void) sizeof ((0) ? 1 : 0), ({ if (0) ; else __assert_fail ("0", "sema.c", 153, __func__); }));
    }
}
void free_ty(ty_t *ty)
{
    switch (ty->kind) {
    case TY_VOID:
    case TY_CHAR:
    case TY_SCHAR:
    case TY_UCHAR:
    case TY_SHORT:
    case TY_USHORT:
    case TY_INT:
    case TY_UINT:
    case TY_LONG:
    case TY_ULONG:
    case TY_LLONG:
    case TY_ULLONG:
    case TY_FLOAT:
    case TY_DOUBLE:
    case TY_LDOUBLE:
    case TY_BOOL:
    case TY_TAG:
        break;
    case TY_POINTER:
        free_ty(ty->pointer.base_ty);
        break;
    case TY_ARRAY:
        free_ty(ty->array.elem_ty);
        break;
    case TY_FUNCTION:
        free_ty(ty->function.ret_ty);
        for (ty_t *param_ty = ty->function.param_tys; param_ty; ) {
            ty_t *tmp = param_ty->next;
            free_ty(param_ty);
            param_ty = tmp;
        }
        break;
    default:
        ((void) sizeof ((0) ? 1 : 0), ({ if (0) ; else __assert_fail ("0", "sema.c", 197, __func__); }));
    }
    free(ty);
}
static void print_ty_r(ty_t *ty)
{
    switch (ty->kind) {
    case TY_VOID: printf("void"); break;
    case TY_CHAR: printf("char"); break;
    case TY_SCHAR: printf("signed char"); break;
    case TY_UCHAR: printf("unsigned char"); break;
    case TY_SHORT: printf("short"); break;
    case TY_USHORT: printf("unsigned short"); break;
    case TY_INT: printf("int"); break;
    case TY_UINT: printf("unsigned int"); break;
    case TY_LONG: printf("long"); break;
    case TY_ULONG: printf("unsigned long"); break;
    case TY_LLONG: printf("long long"); break;
    case TY_ULLONG: printf("unsigned long long"); break;
    case TY_FLOAT: printf("float"); break;
    case TY_DOUBLE: printf("double"); break;
    case TY_LDOUBLE: printf("long double"); break;
    case TY_BOOL: printf("_Bool"); break;
    case TY_TAG:
        switch (ty->tag->kind) {
        case TAG_STRUCT:
            if (ty->tag->name)
                printf("struct %s", ty->tag->name);
            else
                printf("struct");
            break;
        case TAG_UNION:
            if (ty->tag->name)
                printf("union %s", ty->tag->name);
            else
                printf("union");
            break;
        case TAG_ENUM:
            if (ty->tag->name)
                printf("enum %s", ty->tag->name);
            else
                printf("enum");
            break;
        }
        break;
    case TY_POINTER:
        print_ty_r(ty->pointer.base_ty);
        printf("*");
        break;
    case TY_ARRAY:
        print_ty_r(ty->array.elem_ty);
        if (ty->array.cnt < 0)
            printf("[]");
        else
            printf("[%d]", ty->array.cnt);
        break;
    case TY_FUNCTION:
        print_ty_r(ty->function.ret_ty);
        printf("(");
        ty_t *param_ty = ty->function.param_tys;
        while (param_ty) {
            print_ty_r(param_ty);
            if ((param_ty = param_ty->next) || ty->function.var)
                printf(", ");
        }
        if (ty->function.var)
            printf("...");
        printf(")");
        break;
    default:
        ((void) sizeof ((0) ? 1 : 0), ({ if (0) ; else __assert_fail ("0", "sema.c", 294, __func__); }));
    }
}
void print_ty(ty_t *ty)
{
    print_ty_r(ty);
    printf("\n");
}
int ty_align(ty_t *ty)
{
    switch (ty->kind) {
    case TY_CHAR: return 1;
    case TY_SCHAR: return 1;
    case TY_UCHAR: return 1;
    case TY_SHORT: return 2;
    case TY_USHORT: return 2;
    case TY_INT: return 4;
    case TY_UINT: return 4;
    case TY_LONG: return 8;
    case TY_ULONG: return 8;
    case TY_LLONG: return 8;
    case TY_ULLONG: return 8;
    case TY_FLOAT: return 4;
    case TY_DOUBLE: return 8;
    case TY_LDOUBLE: return 16;
    case TY_BOOL: return 1;
    case TY_TAG:
        ((void) sizeof ((ty->tag->defined) ? 1 : 0), ({ if (ty->tag->defined) ; else __assert_fail ("ty->tag->defined", "sema.c", 324, __func__); }));
        switch (ty->tag->kind) {
        case TAG_STRUCT:
        case TAG_UNION:
            return ty->tag->align;
        case TAG_ENUM:
            return 4;
        default:
            ((void) sizeof ((0) ? 1 : 0), ({ if (0) ; else __assert_fail ("0", "sema.c", 332, __func__); }));
        }
    case TY_POINTER:
        return 8;
    case TY_ARRAY:
        return ty_align(ty->array.elem_ty);
    default:
        ((void) sizeof ((0) ? 1 : 0), ({ if (0) ; else __assert_fail ("0", "sema.c", 342, __func__); }));
    }
}
int ty_size(ty_t *ty)
{
    switch (ty->kind) {
    case TY_CHAR: return 1;
    case TY_SCHAR: return 1;
    case TY_UCHAR: return 1;
    case TY_SHORT: return 2;
    case TY_USHORT: return 2;
    case TY_INT: return 4;
    case TY_UINT: return 4;
    case TY_LONG: return 8;
    case TY_ULONG: return 8;
    case TY_LLONG: return 8;
    case TY_ULLONG: return 8;
    case TY_FLOAT: return 4;
    case TY_DOUBLE: return 8;
    case TY_LDOUBLE: return 16;
    case TY_BOOL: return 1;
    case TY_TAG:
        ((void) sizeof ((ty->tag->defined) ? 1 : 0), ({ if (ty->tag->defined) ; else __assert_fail ("ty->tag->defined", "sema.c", 366, __func__); }));
        switch (ty->tag->kind) {
        case TAG_STRUCT:
        case TAG_UNION:
            return ty->tag->size;
        case TAG_ENUM:
            return 4;
        default:
            ((void) sizeof ((0) ? 1 : 0), ({ if (0) ; else __assert_fail ("0", "sema.c", 374, __func__); }));
        }
    case TY_POINTER:
        return 8;
    case TY_ARRAY:
        ((void) sizeof ((ty->array.cnt != -1) ? 1 : 0), ({ if (ty->array.cnt != -1) ; else __assert_fail ("ty->array.cnt != -1", "sema.c", 381, __func__); }));
        return ty_size(ty->array.elem_ty) * ty->array.cnt;
    default:
        ((void) sizeof ((0) ? 1 : 0), ({ if (0) ; else __assert_fail ("0", "sema.c", 385, __func__); }));
    }
}
void sema_init(sema_t *self)
{
    self->scope = ((void *)0);
    sema_enter(self);
}
void sema_free(sema_t *self)
{
    sema_exit(self);
}
void sema_enter(sema_t *self)
{
    scope_t *scope = calloc(1, sizeof *scope);
    if (!scope) abort();
    scope->parent = self->scope;
    self->scope = scope;
}
static void free_members(memb_t *members)
{
    for (memb_t *memb = members; memb;) {
        memb_t *tmp = memb->next;
        free_ty(memb->ty);
        if (memb->name)
            free(memb->name);
        free(memb);
        memb = tmp;
    }
}
void sema_exit(sema_t *self)
{
    scope_t *scope = self->scope;
    self->scope = scope->parent;
    for (sym_t *sym = scope->syms; sym; ) {
        sym_t *tmp = sym->next;
        if (sym->kind != SYM_ENUM_CONST)
            free_ty(sym->ty);
        free(sym->name);
        free(sym);
        sym = tmp;
    }
    for (tag_t *tag = scope->tags; tag; ) {
        tag_t *tmp = tag->next;
        free(tag->name);
        if (tag->members)
            free_members(tag->members);
        free(tag);
        tag = tmp;
    }
    free(scope);
}
void sema_push(sema_t *self, scope_t *scope)
{
    scope->parent = self->scope;
    self->scope = scope;
}
scope_t *sema_pop(sema_t *self)
{
    scope_t *scope = self->scope;
    self->scope = scope->parent;
    return scope;
}
static int sym_kind(sema_t *self, int sc)
{
    switch (sc) {
    case TK_TYPEDEF:
        return SYM_TYPEDEF;
    case TK_EXTERN:
        return SYM_EXTERN;
    case TK_STATIC:
        return SYM_STATIC;
    case TK_AUTO:
    case TK_REGISTER:
        return SYM_LOCAL;
    case -1:
        if (self->scope->parent)
            return SYM_LOCAL;
        else
            return SYM_EXTERN;
    default:
        ((void) sizeof ((0) ? 1 : 0), ({ if (0) ; else __assert_fail ("0", "sema.c", 483, __func__); }));
    }
}
sym_t *sema_declare(sema_t *self, int sc, ty_t *ty, char *name)
{
    sym_t *sym;
    for (sym = self->scope->syms; sym; sym = sym->next)
        if (!strcmp(sym->name, name))
            break;
    if (sym) {
        int new_kind = sym_kind(self, sc);
        switch (sym->kind) {
        case SYM_EXTERN:
        case SYM_STATIC:
            if (new_kind == SYM_EXTERN) {
                break;
            }
            if (new_kind == SYM_STATIC) {
                sym->kind = SYM_STATIC;
                break;
            }
        default:
            err("Invalid re-declaration of %s", name);
        }
        printf("Re-declaration of %s\n", name);
        return sym;
    }
    sym = calloc(1, sizeof *sym);
    if (!sym) abort();
    sym->next = self->scope->syms;
    self->scope->syms = sym;
    sym->kind = sym_kind(self, sc);
    if (sym->kind != SYM_TYPEDEF && ty->kind == TY_VOID)
        err("Tried to declare %s with type void", name);
    sym->ty = ty;
    sym->name = name;
    printf("Declare %s as ", name);
    print_ty(ty);
    if (sym->kind == SYM_LOCAL) {
        self->offset = align(self->offset, ty_align(ty));
        sym->offset = self->offset;
        self->offset += ty_size(ty);
    } else {
        sym->offset = -1;
    }
    return sym;
}
sym_t *sema_declare_enum_const(sema_t *self, char *name, val_t val)
{
    for (sym_t *sym = self->scope->syms; sym; sym = sym->next)
        if (!strcmp(sym->name, name))
            err("Invalid re-declaration of %s", name);
    sym_t *sym = calloc(1, sizeof *sym);
    if (!sym) abort();
    sym->next = self->scope->syms;
    self->scope->syms = sym;
    sym->kind = SYM_ENUM_CONST;
    sym->name = name;
    sym->val = val;
    return sym;
}
sym_t *sema_lookup(sema_t *self, const char *name)
{
    for (scope_t *scope = self->scope; scope; scope = scope->parent)
        for (sym_t *sym = scope->syms; sym; sym = sym->next)
            if (!strcmp(sym->name, name))
                return sym;
    return ((void *)0);
}
ty_t *sema_findtypedef(sema_t *self, const char *name)
{
    struct sym *sym = sema_lookup(self, name);
    if (sym && sym->kind == SYM_TYPEDEF) {
        return clone_ty(sym->ty);
    }
    return ((void *)0);
}
static tag_t *create_tag(sema_t *self, int kind, const char *name)
{
    tag_t *tag = calloc(1, sizeof *tag);
    if (!tag) abort();
    tag->next = self->scope->tags;
    self->scope->tags = tag;
    tag->kind = kind;
    tag->name = name ? strdup(name) : ((void *)0);
    return tag;
}
tag_t *sema_forward_declare_tag(sema_t *self, int kind, const char *name)
{
    tag_t *tag = ((void *)0);
    if (name)
        for (scope_t *scope = self->scope; scope; scope = scope->parent)
            for (tag = scope->tags; tag; tag = tag->next)
                if (tag->name && !strcmp(tag->name, name))
                    goto found;
found:
    if (tag) {
        if (tag->kind != kind)
            err("Tag referred to with the wrong keyword");
    } else {
        return create_tag(self, kind, name);
    }
    return tag;
}
tag_t *sema_define_tag(sema_t *self, int kind, const char *name)
{
    tag_t *tag = ((void *)0);
    if (name)
        for (tag = self->scope->tags; tag; tag = tag->next)
            if (tag->name && !strcmp(tag->name, name))
                break;
    if (tag) {
        if (tag->kind != kind)
            err("Tag referred to with the wrong keyword");
        if (tag->defined)
            err("Re-definition of tag %s", name);
    } else {
        tag = create_tag(self, kind, name);
    }
    tag->defined = 1;
    return tag;
}
static expr_t *make_expr(int kind)
{
    expr_t *expr = calloc(1, sizeof *expr);
    if (!expr) abort();
    expr->kind = kind;
    return expr;
}
expr_t *make_sym(sema_t *self, const char *name)
{
    sym_t *sym = sema_lookup(self, name);
    if (!sym)
        err("Undeclared identifier %s", name);
    expr_t *expr;
    switch (sym->kind) {
    case SYM_TYPEDEF:
        err("Invalid use of typedef name %s", name);
    case SYM_EXTERN:
    case SYM_STATIC:
        expr = make_expr(EXPR_GLOBAL);
        expr->ty = clone_ty(sym->ty);
        expr->str = strdup(name);
        break;
    case SYM_LOCAL:
        expr = make_expr(EXPR_LOCAL);
        expr->ty = clone_ty(sym->ty);
        expr->offset = sym->offset;
        break;
    case SYM_ENUM_CONST:
        expr = make_const(make_ty(TY_INT), sym->val);
        break;
    }
    return expr;
}
expr_t *make_const(ty_t *ty, val_t val)
{
    expr_t *expr = make_expr(EXPR_CONST);
    expr->ty = ty;
    expr->val = val;
    return expr;
}
expr_t *make_str_lit(const char *str)
{
    expr_t *expr = make_expr(EXPR_STR_LIT);
    expr->ty = make_array(make_ty(TY_CHAR), strlen(str) + 1);
    expr->str = strdup(str);
    return expr;
}
static _Bool is_integer_ty(ty_t *ty)
{
    switch (ty->kind) {
    case TY_CHAR:
    case TY_SCHAR:
    case TY_UCHAR:
    case TY_SHORT:
    case TY_USHORT:
    case TY_INT:
    case TY_UINT:
    case TY_LONG:
    case TY_ULONG:
    case TY_LLONG:
    case TY_ULLONG:
    case TY_BOOL:
        return 1;
    case TY_TAG:
        return ty->tag->kind == TAG_ENUM && ty->tag->defined;
    case TY_FLOAT:
    case TY_DOUBLE:
    case TY_LDOUBLE:
    case TY_POINTER:
    case TY_ARRAY:
    case TY_FUNCTION:
        return 0;
    default:
        ((void) sizeof ((0) ? 1 : 0), ({ if (0) ; else __assert_fail ("0", "sema.c", 731, __func__); }));
    }
}
static _Bool is_arith_ty(ty_t *ty)
{
    switch (ty->kind) {
    case TY_CHAR:
    case TY_SCHAR:
    case TY_UCHAR:
    case TY_SHORT:
    case TY_USHORT:
    case TY_INT:
    case TY_UINT:
    case TY_LONG:
    case TY_ULONG:
    case TY_LLONG:
    case TY_ULLONG:
    case TY_BOOL:
    case TY_FLOAT:
    case TY_DOUBLE:
    case TY_LDOUBLE:
        return 1;
    case TY_TAG:
        return ty->tag->kind == TAG_ENUM && ty->tag->defined;
    case TY_POINTER:
    case TY_ARRAY:
    case TY_FUNCTION:
        return 0;
    default:
        ((void) sizeof ((0) ? 1 : 0), ({ if (0) ; else __assert_fail ("0", "sema.c", 762, __func__); }));
    }
}
static _Bool is_scalar_ty(ty_t *ty)
{
    switch (ty->kind) {
    case TY_CHAR:
    case TY_SCHAR:
    case TY_UCHAR:
    case TY_SHORT:
    case TY_USHORT:
    case TY_INT:
    case TY_UINT:
    case TY_LONG:
    case TY_ULONG:
    case TY_LLONG:
    case TY_ULLONG:
    case TY_BOOL:
    case TY_FLOAT:
    case TY_DOUBLE:
    case TY_LDOUBLE:
    case TY_POINTER:
    case TY_ARRAY:
    case TY_FUNCTION:
        return 1;
    case TY_TAG:
        return ty->tag->kind == TAG_ENUM && ty->tag->defined;
    default:
        ((void) sizeof ((0) ? 1 : 0), ({ if (0) ; else __assert_fail ("0", "sema.c", 795, __func__); }));
    }
}
static ty_t *find_ptr_base(ty_t *ty)
{
    if (ty->kind == TY_POINTER)
        return ty->pointer.base_ty;
    else if (ty->kind == TY_ARRAY)
        return ty->array.elem_ty;
    else if (ty->kind == TY_FUNCTION)
        return ty;
    else
        return ((void *)0);
}
expr_t *make_unary(int kind, expr_t *arg1)
{
    ty_t *ty;
    switch (kind) {
    case EXPR_REF:
        ty = make_pointer(arg1->ty);
        break;
    case EXPR_DREF:
        if (!(ty = find_ptr_base(arg1->ty)))
            err("Pointer type required");
        break;
    case EXPR_POS:
        if (!is_arith_ty((ty = arg1->ty)))
            err("Arihmetic type required");
        return arg1;
    case EXPR_NEG:
        if (!is_arith_ty((ty = arg1->ty)))
            err("Arihmetic type required");
        break;
    case EXPR_NOT:
        if (!is_integer_ty((ty = arg1->ty)))
            err("Integer type required");
        break;
    case EXPR_LNOT:
        if (!is_scalar_ty((ty = arg1->ty)))
        ty = make_ty(TY_INT);
        break;
    case EXPR_INIT:
        ty = ((void *)0);
        break;
    default:
        ((void) sizeof ((0) ? 1 : 0), ({ if (0) ; else __assert_fail ("0", "sema.c", 845, __func__); }));
    }
    expr_t *expr = make_expr(kind);
    expr->ty = ty;
    expr->arg1 = arg1;
    return expr;
}
expr_t *make_memb_expr(expr_t *arg1, const char *name)
{
    expr_t *expr = make_expr(EXPR_MEMB);
    expr->arg1 = arg1;
    if (arg1->ty->kind != TY_TAG)
        goto err;
    tag_t *tag = arg1->ty->tag;
    if (tag->kind != TAG_STRUCT && tag->kind != TAG_UNION)
        goto err;
    if (!tag->defined)
        goto err;
    for (memb_t *memb = tag->members; memb; memb = memb->next)
        if (!strcmp(memb->name, name)) {
            expr->ty = clone_ty(memb->ty);
            expr->offset = memb->offset;
            return expr;
        }
err:
    print_ty_r(arg1->ty);
    err(" has no member %s\n", name);
}
expr_t *make_binary(int kind, expr_t *arg1, expr_t *arg2)
{
    expr_t *expr = make_expr(kind);
    switch (kind) {
    case EXPR_CALL:
    {
        ty_t *func_ty = find_ptr_base(arg1->ty);
        if (!func_ty || func_ty->kind != TY_FUNCTION)
            err("Non-function object called");
        expr->ty = func_ty->function.ret_ty;
        break;
    }
    case EXPR_MUL:
    case EXPR_DIV:
    case EXPR_MOD:
        expr->ty = arg1->ty;
        break;
    case EXPR_ADD:
    {
        ty_t *ty;
        if ((ty = find_ptr_base(arg1->ty))) {
            if (!is_integer_ty(arg2->ty))
                err("Pointer offset must be integer");
            arg2 = make_binary(EXPR_MUL, arg2,
                make_const(make_ty(TY_INT), ty_size(ty)));
            expr->ty = make_pointer(ty);
        } else if ((ty = find_ptr_base(arg2->ty))) {
            if (!is_integer_ty(arg1->ty))
                err("Pointer offset must be integer");
            arg1 = make_binary(EXPR_MUL, arg1,
                make_const(make_ty(TY_INT), ty_size(ty)));
            expr->ty = make_pointer(ty);
        } else {
            if (!is_arith_ty(arg1->ty) || !is_arith_ty(arg2->ty))
                err("Arihmetic type required");
            expr->ty = arg1->ty;
        }
        break;
    }
    case EXPR_SUB:
        expr->ty = arg1->ty;
        break;
    case EXPR_LSH:
    case EXPR_RSH:
    case EXPR_AND:
    case EXPR_XOR:
    case EXPR_OR:
        expr->ty = arg1->ty;
        break;
    case EXPR_LT:
    case EXPR_GT:
    case EXPR_LE:
    case EXPR_GE:
    case EXPR_EQ:
    case EXPR_NE:
    case EXPR_LAND:
    case EXPR_LOR:
        expr->ty = make_ty(TY_INT);
        break;
    case EXPR_AS:
        expr->ty = arg1->ty;
        break;
    case EXPR_SEQ:
        expr->ty = arg2->ty;
        break;
    default:
        ((void) sizeof ((0) ? 1 : 0), ({ if (0) ; else __assert_fail ("0", "sema.c", 956, __func__); }));
    }
    expr->arg1 = arg1;
    expr->arg2 = arg2;
    return expr;
}
expr_t *make_trinary(int kind, expr_t *arg1, expr_t *arg2, expr_t *arg3)
{
    expr_t *expr = make_expr(kind);
    ((void) sizeof ((kind == EXPR_COND) ? 1 : 0), ({ if (kind == EXPR_COND) ; else __assert_fail ("kind == EXPR_COND", "sema.c", 966, __func__); }));
    expr->ty = arg2->ty;
    expr->arg1 = arg1;
    expr->arg2 = arg2;
    expr->arg3 = arg3;
    return expr;
}
stmt_t *make_stmt(int kind)
{
    stmt_t *stmt = calloc(1, sizeof *stmt);
    if (!stmt) abort();
    stmt->kind = kind;
    return stmt;
}
