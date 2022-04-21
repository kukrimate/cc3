
extern void __assert_fail (const char *__assertion, const char *__file,
      unsigned int __line, const char *__function)
     ;
extern void __assert_perror_fail (int __errnum, const char *__file,
      unsigned int __line, const char *__function)
     ;
extern void __assert (const char *__assertion, const char *__file, int __line)
     ;


extern int *__errno_location (void) ;

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
typedef struct sym sym_t;
struct ty {
    ty_t *next;
    sym_t *sym;
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
    const char *func_name;
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
    EXPR_STMT,
};
typedef struct expr expr_t;
typedef struct stmt stmt_t;
struct expr {
    expr_t *next;
    int kind;
    ty_t *ty;
    int offset;
    char *str;
    val_t val;
    expr_t *arg1, *arg2, *arg3;
    stmt_t *body;
};
expr_t *make_sym(sema_t *self, const char *name);
expr_t *make_const(ty_t *ty, val_t val);
expr_t *make_str_lit(const char *str);
expr_t *make_stmt_expr(stmt_t *body);
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
static inline tk_t *peek(cc3_t *self, int i)
{
    return lex_tok(&self->lexer, i);
}
static inline void adv(cc3_t *self)
{
    lex_adv(&self->lexer);
}
static inline tk_t *next(cc3_t *self)
{
    tk_t *tk = lex_tok(&self->lexer, 0);
    lex_adv(&self->lexer);
    return tk;
}
static inline tk_t *want(cc3_t *self, int want_type)
{
    tk_t *tk = lex_tok(&self->lexer, 0);
    if (tk->type != want_type)
        err("Unexpected token %s", tk_str(tk));
    lex_adv(&self->lexer);
    return tk;
}
static inline tk_t *maybe_want(cc3_t *self, int want_type)
{
    tk_t *tk = lex_tok(&self->lexer, 0);
    if (tk->type == want_type) {
        lex_adv(&self->lexer);
        return tk;
    }
    return ((void *)0);
}
static expr_t *primary_expression(cc3_t *self);
static expr_t *postfix_expression(cc3_t *self);
static expr_t *unary_expression(cc3_t *self);
static expr_t *cast_expression(cc3_t *self);
static expr_t *multiplicative_expression(cc3_t *self);
static expr_t *additive_expression(cc3_t *self);
static expr_t *shift_expression(cc3_t *self);
static expr_t *relational_expression(cc3_t *self);
static expr_t *equality_expression(cc3_t *self);
static expr_t *and_expression(cc3_t *self);
static expr_t *xor_expression(cc3_t *self);
static expr_t *or_expression(cc3_t *self);
static expr_t *land_expression(cc3_t *self);
static expr_t *lor_expression(cc3_t *self);
static expr_t *conditional_expression(cc3_t *self);
static expr_t *assignment_expression(cc3_t *self);
static expr_t *expression(cc3_t *self);
static int constant_expression(cc3_t *self);
static stmt_t *read_block(cc3_t *self);
expr_t *primary_expression(cc3_t *self)
{
    tk_t *tk = next(self);
    expr_t *expr;
    switch (tk->type) {
    case TK_IDENTIFIER:
        expr = make_sym(&self->sema, tk_str(tk));
        break;
    case TK_CONSTANT:
        expr = make_const(make_ty(TY_INT), tk->val);
        break;
    case TK_STR_LIT:
        expr = make_str_lit(tk->str.data);
        break;
    case TK_LPAREN:
        if (maybe_want(self, TK_LCURLY)) {
            stmt_t *body = read_block(self);
            want(self, TK_RPAREN);
            expr = make_stmt_expr(body);
        } else {
            expr = expression(self);
            want(self, TK_RPAREN);
        }
        break;
    default:
        err("Invalid primary expression %s", tk_str(tk));
    }
    return expr;
}
expr_t *postfix_expression(cc3_t *self)
{
    expr_t *expr = primary_expression(self);
    for (;;) {
        if (maybe_want(self, TK_LSQ)) {
            expr = make_unary(EXPR_DREF,
                make_binary(EXPR_ADD, expr, expression(self)));
            want(self, TK_RSQ);
            continue;
        }
        if (maybe_want(self, TK_LPAREN)) {
            expr_t *args = ((void *)0), **tail = &args;
            if (!maybe_want(self, TK_RPAREN)) {
                do {
                    *tail = assignment_expression(self);
                    tail = &(*tail)->next;
                } while (maybe_want(self, TK_COMMA));
                want(self, TK_RPAREN);
            }
            expr = make_binary(EXPR_CALL, expr, args);
            continue;
        }
        if (maybe_want(self, TK_DOT)) {
            const char *name = tk_str(want(self, TK_IDENTIFIER));
            expr = make_memb_expr(expr, name);
            continue;
        }
        if (maybe_want(self, TK_ARROW)) {
            const char *name = tk_str(want(self, TK_IDENTIFIER));
            expr = make_memb_expr(make_unary(EXPR_DREF, expr), name);
            continue;
        }
        if (maybe_want(self, TK_INCR)) {
            ((void) sizeof ((0) ? 1 : 0), ({ if (0) ; else __assert_fail ("0", "parse.c", 130, __func__); }));
            continue;
        }
        if (maybe_want(self, TK_DECR)) {
            ((void) sizeof ((0) ? 1 : 0), ({ if (0) ; else __assert_fail ("0", "parse.c", 134, __func__); }));
            continue;
        }
        return expr;
    }
}
static _Bool is_declaration_specifier(cc3_t *self, tk_t *tk);
static ty_t *type_name(cc3_t *self);
expr_t *unary_expression(cc3_t *self)
{
    if (maybe_want(self, TK_INCR)) {
        expr_t *arg = unary_expression(self);
        return make_binary(EXPR_AS, arg,
            make_binary(EXPR_ADD, arg, make_const(make_ty(TY_INT), 1)));
    } else if (maybe_want(self, TK_DECR)) {
        expr_t *arg = unary_expression(self);
        return make_binary(EXPR_AS, arg,
            make_binary(EXPR_SUB, arg, make_const(make_ty(TY_INT), 1)));
    } else if (maybe_want(self, TK_AND)) {
        return make_unary(EXPR_REF, cast_expression(self));
    } else if (maybe_want(self, TK_MUL)) {
        return make_unary(EXPR_DREF, cast_expression(self));
    } else if (maybe_want(self, TK_ADD)) {
        return cast_expression(self);
    } else if (maybe_want(self, TK_SUB)) {
        return make_unary(EXPR_NEG, cast_expression(self));
    } else if (maybe_want(self, TK_NOT)) {
        return make_unary(EXPR_NOT, cast_expression(self));
    } else if (maybe_want(self, TK_LNOT)) {
        return make_unary(EXPR_LNOT, cast_expression(self));
    } else if (maybe_want(self, TK_SIZEOF)) {
        ty_t *ty;
        if (peek(self, 0)->type == TK_LPAREN
                && is_declaration_specifier(self, peek(self, 1))) {
            adv(self);
            ty = type_name(self);
            want(self, TK_RPAREN);
        } else {
            ty = unary_expression(self)->ty;
        }
        return make_const(make_ty(TY_ULONG), ty_size(ty));
    } else {
        return postfix_expression(self);
    }
}
expr_t *cast_expression(cc3_t *self)
{
    if (peek(self, 0)->type == TK_LPAREN
                && is_declaration_specifier(self, peek(self, 1))) {
        adv(self);
        type_name(self);
        want(self, TK_RPAREN);
        return cast_expression(self);
    } else {
        return unary_expression(self);
    }
}
expr_t *multiplicative_expression(cc3_t *self)
{
    expr_t *lhs = cast_expression(self);
    for (;;)
        if (maybe_want(self, TK_MUL))
            lhs = make_binary(EXPR_MUL, lhs, cast_expression(self));
        else if (maybe_want(self, TK_DIV))
            lhs = make_binary(EXPR_DIV, lhs, cast_expression(self));
        else if (maybe_want(self, TK_MOD))
            lhs = make_binary(EXPR_MOD, lhs, cast_expression(self));
        else
            return lhs;
}
expr_t *additive_expression(cc3_t *self)
{
    expr_t *lhs = multiplicative_expression(self);
    for (;;)
        if (maybe_want(self, TK_ADD))
            lhs = make_binary(EXPR_ADD, lhs, multiplicative_expression(self));
        else if (maybe_want(self, TK_SUB))
            lhs = make_binary(EXPR_SUB, lhs, multiplicative_expression(self));
        else
            return lhs;
}
expr_t *shift_expression(cc3_t *self)
{
    expr_t *lhs = additive_expression(self);
    for (;;)
        if (maybe_want(self, TK_LSH))
            lhs = make_binary(EXPR_LSH, lhs, additive_expression(self));
        else if (maybe_want(self, TK_RSH))
            lhs = make_binary(EXPR_RSH, lhs, additive_expression(self));
        else
            return lhs;
}
expr_t *relational_expression(cc3_t *self)
{
    expr_t *lhs = shift_expression(self);
    for (;;)
        if (maybe_want(self, TK_LT))
            lhs = make_binary(EXPR_LT, lhs, shift_expression(self));
        else if (maybe_want(self, TK_GT))
            lhs = make_binary(EXPR_GT, lhs, shift_expression(self));
        else if (maybe_want(self, TK_LE))
            lhs = make_binary(EXPR_LE, lhs, shift_expression(self));
        else if (maybe_want(self, TK_GE))
            lhs = make_binary(EXPR_GE, lhs, shift_expression(self));
        else
            return lhs;
}
expr_t *equality_expression(cc3_t *self)
{
    expr_t *lhs = relational_expression(self);
    for (;;)
        if (maybe_want(self, TK_EQ))
            lhs = make_binary(EXPR_EQ, lhs, relational_expression(self));
        else if (maybe_want(self, TK_NE))
            lhs = make_binary(EXPR_NE, lhs, relational_expression(self));
        else
            return lhs;
}
expr_t *and_expression(cc3_t *self)
{
    expr_t *lhs = equality_expression(self);
    while (maybe_want(self, TK_AND))
        lhs = make_binary(EXPR_AND, lhs, equality_expression(self));
    return lhs;
}
expr_t *xor_expression(cc3_t *self)
{
    expr_t *lhs = and_expression(self);
    while (maybe_want(self, TK_XOR))
        lhs = make_binary(EXPR_XOR, lhs, and_expression(self));
    return lhs;
}
expr_t *or_expression(cc3_t *self)
{
    expr_t *lhs = xor_expression(self);
    while (maybe_want(self, TK_OR))
        lhs = make_binary(EXPR_OR, lhs, xor_expression(self));
    return lhs;
}
expr_t *land_expression(cc3_t *self)
{
    expr_t *lhs = or_expression(self);
    while (maybe_want(self, TK_LAND))
        lhs = make_binary(EXPR_LAND, lhs, or_expression(self));
    return lhs;
}
expr_t *lor_expression(cc3_t *self)
{
    expr_t *lhs = land_expression(self);
    while (maybe_want(self, TK_LOR))
        lhs = make_binary(EXPR_LOR, lhs, land_expression(self));
    return lhs;
}
expr_t *conditional_expression(cc3_t *self)
{
    expr_t *lhs = lor_expression(self);
    if (!maybe_want(self, TK_COND))
        return lhs;
    expr_t *mid = expression(self);
    want(self, TK_COLON);
    return make_trinary(EXPR_COND, lhs, mid, conditional_expression(self));
}
expr_t *assignment_expression(cc3_t *self)
{
    expr_t *lhs = conditional_expression(self);
    if (maybe_want(self, TK_AS))
        return make_binary(EXPR_AS, lhs, assignment_expression(self));
    else if (maybe_want(self, TK_MUL_AS))
        return make_binary(EXPR_AS, lhs,
                make_binary(EXPR_MUL, lhs, assignment_expression(self)));
    else if (maybe_want(self, TK_DIV_AS))
        return make_binary(EXPR_AS, lhs,
                make_binary(EXPR_DIV, lhs, assignment_expression(self)));
    else if (maybe_want(self, TK_MOD_AS))
        return make_binary(EXPR_AS, lhs,
                make_binary(EXPR_MOD, lhs, assignment_expression(self)));
    else if (maybe_want(self, TK_ADD_AS))
        return make_binary(EXPR_AS, lhs,
                make_binary(EXPR_ADD, lhs, assignment_expression(self)));
    else if (maybe_want(self, TK_SUB_AS))
        return make_binary(EXPR_AS, lhs,
                make_binary(EXPR_SUB, lhs, assignment_expression(self)));
    else if (maybe_want(self, TK_LSH_AS))
        return make_binary(EXPR_AS, lhs,
                make_binary(EXPR_LSH, lhs, assignment_expression(self)));
    else if (maybe_want(self, TK_RSH_AS))
        return make_binary(EXPR_AS, lhs,
                make_binary(EXPR_RSH, lhs, assignment_expression(self)));
    else if (maybe_want(self, TK_AND_AS))
        return make_binary(EXPR_AS, lhs,
                make_binary(EXPR_AND, lhs, assignment_expression(self)));
    else if (maybe_want(self, TK_XOR_AS))
        return make_binary(EXPR_AS, lhs,
                make_binary(EXPR_XOR, lhs, assignment_expression(self)));
    else if (maybe_want(self, TK_OR_AS))
        return make_binary(EXPR_AS, lhs,
                make_binary(EXPR_OR, lhs, assignment_expression(self)));
    else
        return lhs;
}
expr_t *expression(cc3_t *self)
{
    expr_t *lhs = assignment_expression(self);
    while (maybe_want(self, TK_COMMA))
        lhs = make_binary(EXPR_SEQ, lhs, assignment_expression(self));
    return lhs;
}
int constant_expression(cc3_t *self)
{
    expr_t *expr = conditional_expression(self);
    if (expr->kind != EXPR_CONST)
        err("Expected constant expression");
    return expr->val;
}
static expr_t *initializer_list(cc3_t *self);
static expr_t *initializer(cc3_t *self)
{
    if (maybe_want(self, TK_LCURLY))
        return initializer_list(self);
    else
        return assignment_expression(self);
}
static expr_t *initializer_list(cc3_t *self)
{
    expr_t *head = ((void *)0), **tail = &head;
    for (;;) {
        *tail = initializer(self);
        tail = &(*tail)->next;
        if (maybe_want(self, TK_COMMA)) {
            if (maybe_want(self, TK_RCURLY))
                break;
        } else {
            want(self, TK_RCURLY);
            break;
        }
    }
    return make_unary(EXPR_INIT, head);
}
static ty_t *declaration_specifiers(cc3_t *self, int *out_sc);
static ty_t *declarator(cc3_t *self, ty_t *ty, _Bool allow_abstract, char **out_name);
static memb_t *member_list(cc3_t *self)
{
    memb_t *members = ((void *)0), **tail = &members;
    do {
        int sc;
        ty_t *base_ty = declaration_specifiers(self, &sc);
        if (!base_ty)
            err("Expected declaration in struct/union");
        if (sc != -1)
            err("Storge class not allowed in struct/union");
        if (!maybe_want(self, TK_SEMICOLON)) {
            do {
                char *name;
                ty_t *ty = declarator(self, clone_ty(base_ty), 0, &name);
                *tail = make_memb(ty, name);
                tail = &(*tail)->next;
            } while (maybe_want(self, TK_COMMA));
            want(self, TK_SEMICOLON);
        } else {
            if (base_ty->kind != TY_TAG
                    || (base_ty->tag->kind != TAG_STRUCT
                        && base_ty->tag->kind != TAG_UNION)
                    || !base_ty->tag->defined)
                err("Invalid anonymous member");
            *tail = make_memb(clone_ty(base_ty), ((void *)0));
            tail = &(*tail)->next;
        }
        free_ty(base_ty);
    } while (!maybe_want(self, TK_RCURLY));
    return members;
}
static tag_t *struct_specifier(cc3_t *self)
{
    tk_t *tk;
    tag_t *tag;
    if ((tk = maybe_want(self, TK_IDENTIFIER))) {
        if (maybe_want(self, TK_LCURLY)) {
            tag = sema_define_tag(&self->sema, TAG_STRUCT, tk_str(tk));
            tag->members = pack_struct(member_list(self), &tag->align, &tag->size);
        } else {
            tag = sema_forward_declare_tag(&self->sema, TAG_STRUCT, tk_str(tk));
        }
    } else {
        want(self, TK_LCURLY);
        tag = sema_define_tag(&self->sema, TAG_STRUCT, ((void *)0));
        tag->members = pack_struct(member_list(self), &tag->align, &tag->size);
    }
    return tag;
}
static tag_t *union_specifier(cc3_t *self)
{
    tk_t *tk;
    tag_t *tag;
    if ((tk = maybe_want(self, TK_IDENTIFIER))) {
        if (maybe_want(self, TK_LCURLY)) {
            tag = sema_define_tag(&self->sema, TAG_UNION, tk_str(tk));
            tag->members = pack_union(member_list(self), &tag->align, &tag->size);
        } else {
            tag = sema_forward_declare_tag(&self->sema, TAG_UNION, tk_str(tk));
        }
    } else {
        want(self, TK_LCURLY);
        tag = sema_define_tag(&self->sema, TAG_UNION, ((void *)0));
        tag->members = pack_union(member_list(self), &tag->align, &tag->size);
    }
    return tag;
}
static void enumerator_list(cc3_t *self)
{
    val_t cur = 0;
    for (;; ++cur) {
        char *name = strdup(tk_str(want(self, TK_IDENTIFIER)));
        if (maybe_want(self, TK_AS))
            cur = constant_expression(self);
        sema_declare_enum_const(&self->sema, name, cur);
        if (maybe_want(self, TK_COMMA)) {
            if (maybe_want(self, TK_RCURLY))
                return;
        } else {
            want(self, TK_RCURLY);
            return;
        }
    }
}
static tag_t *enum_specifier(cc3_t *self)
{
    tk_t *tk;
    tag_t *tag;
    if ((tk = maybe_want(self, TK_IDENTIFIER))) {
        if (maybe_want(self, TK_LCURLY)) {
            sema_define_tag(&self->sema, TAG_ENUM, tk_str(tk));
            enumerator_list(self);
        } else {
            sema_forward_declare_tag(&self->sema, TAG_ENUM, tk_str(tk));
        }
    } else {
        want(self, TK_LCURLY);
        tag = sema_define_tag(&self->sema, TAG_ENUM, ((void *)0));
        enumerator_list(self);
    }
    return tag;
}
enum {
    TS_VOID, TS_CHAR, TS_SHORT, TS_INT, TS_LONG, TS_FLOAT, TS_DOUBLE,
    TS_SIGNED, TS_UNSIGNED, TS_BOOL, TS_COMPLEX, TS_IMAGINARY, NUM_TS
};
static ty_t *decode_ts(int ts[static NUM_TS])
{
    static const struct {
        int ts[NUM_TS];
        int kind;
    } maps[] = {
        { { [TS_VOID] = 1 }, TY_VOID },
        { { [TS_CHAR] = 1 }, TY_CHAR },
        { { [TS_SIGNED] = 1, [TS_CHAR] = 1 }, TY_SCHAR },
        { { [TS_UNSIGNED] = 1, [TS_CHAR] = 1 }, TY_UCHAR },
        { { [TS_SHORT] = 1 }, TY_SHORT },
        { { [TS_SIGNED] = 1, [TS_SHORT] = 1 }, TY_SHORT },
        { { [TS_SHORT] = 1, [TS_INT] = 1 }, TY_SHORT },
        { { [TS_SIGNED] = 1, [TS_SHORT] = 1, [TS_INT] = 1 }, TY_SHORT },
        { { [TS_UNSIGNED] = 1, [TS_SHORT] = 1 }, TY_USHORT },
        { { [TS_UNSIGNED] = 1, [TS_SHORT] = 1, [TS_INT] = 1 }, TY_USHORT },
        { { [TS_INT] = 1 }, TY_INT },
        { { [TS_SIGNED] = 1 }, TY_INT },
        { { [TS_SIGNED] = 1, [TS_INT] = 1 }, TY_INT },
        { { [TS_UNSIGNED] = 1 }, TY_UINT },
        { { [TS_UNSIGNED] = 1, [TS_INT] = 1 }, TY_UINT },
        { { [TS_LONG] = 1 }, TY_LONG },
        { { [TS_SIGNED] = 1, [TS_LONG] = 1 }, TY_LONG },
        { { [TS_LONG] = 1, [TS_INT] = 1 }, TY_LONG },
        { { [TS_SIGNED] = 1, [TS_LONG] = 1, [TS_INT] = 1 }, TY_LONG },
        { { [TS_UNSIGNED] = 1, [TS_LONG] = 1 }, TY_ULONG },
        { { [TS_UNSIGNED] = 1, [TS_LONG] = 1, [TS_INT] = 1 }, TY_ULONG },
        { { [TS_LONG] = 2 }, TY_LLONG },
        { { [TS_SIGNED] = 1, [TS_LONG] = 2 }, TY_LLONG },
        { { [TS_LONG] = 2, [TS_INT] = 1 }, TY_LLONG },
        { { [TS_SIGNED] = 1, [TS_LONG] = 2, [TS_INT] = 1 }, TY_LLONG },
        { { [TS_UNSIGNED] = 1, [TS_LONG] = 2 }, TY_ULLONG },
        { { [TS_UNSIGNED] = 1, [TS_LONG] = 2, [TS_INT] = 1 }, TY_ULLONG },
        { { [TS_FLOAT] = 1 }, TY_FLOAT },
        { { [TS_DOUBLE] = 1 }, TY_DOUBLE },
        { { [TS_LONG] = 1, [TS_DOUBLE] = 1 }, TY_LDOUBLE },
        { { [TS_BOOL] = 1 }, TY_BOOL },
    };
    for (int i = 0; i < sizeof maps / sizeof *maps; ++i)
        if (memcmp(ts, maps[i].ts, sizeof maps[i].ts) == 0)
            return make_ty(maps[i].kind);
    err("Provided type specifiers do not name a valid type");
}
ty_t *declaration_specifiers(cc3_t *self, int *out_sc)
{
    _Bool match = 0;
    *out_sc = -1;
    ty_t *ty = ((void *)0);
    _Bool had_ts = 0;
    int ts[NUM_TS] = {0};
    for (;;) {
        tk_t *tk = peek(self, 0);
        switch (tk->type) {
        case TK_TYPEDEF:
        case TK_EXTERN:
        case TK_STATIC:
        case TK_AUTO:
        case TK_REGISTER:
            if (*out_sc != -1)
                err("Duplicate storage class");
            *out_sc = tk->type;
            break;
        case TK_CONST:
        case TK_RESTRICT:
        case TK_VOLATILE:
            break;
        case TK_INLINE:
            break;
        case TK_VOID:
            if (ty) err("Invalid type specifiers");
            had_ts = 1;
            ++ts[TS_VOID];
            break;
        case TK_CHAR:
            if (ty) err("Invalid type specifiers");
            had_ts = 1;
            ++ts[TS_CHAR];
            break;
        case TK_SHORT:
            if (ty) err("Invalid type specifiers");
            had_ts = 1;
            ++ts[TS_SHORT];
            break;
        case TK_INT:
            if (ty) err("Invalid type specifiers");
            had_ts = 1;
            ++ts[TS_INT];
            break;
        case TK_LONG:
            if (ty) err("Invalid type specifiers");
            had_ts = 1;
            ++ts[TS_LONG];
            break;
        case TK_FLOAT:
            if (ty) err("Invalid type specifiers");
            had_ts = 1;
            ++ts[TS_FLOAT];
            break;
        case TK_DOUBLE:
            if (ty) err("Invalid type specifiers");
            had_ts = 1;
            ++ts[TS_DOUBLE];
            break;
        case TK_SIGNED:
            if (ty) err("Invalid type specifiers");
            had_ts = 1;
            ++ts[TS_SIGNED];
            break;
        case TK_UNSIGNED:
            if (ty) err("Invalid type specifiers");
            had_ts = 1;
            ++ts[TS_UNSIGNED];
            break;
        case TK_BOOL:
            if (ty) err("Invalid type specifiers");
            had_ts = 1;
            ++ts[TS_BOOL];
            break;
        case TK_COMPLEX:
            if (ty) err("Invalid type specifiers");
            had_ts = 1;
            ++ts[TS_COMPLEX];
            break;
        case TK_IMAGINARY:
            if (ty) err("Invalid type specifiers");
            had_ts = 1;
            ++ts[TS_IMAGINARY];
            break;
        case TK_STRUCT:
            if (ty || had_ts) err("Invalid type specifiers");
            adv(self);
            ty = make_ty_tag(struct_specifier(self));
            goto set_match;
        case TK_UNION:
            if (ty || had_ts) err("Invalid type specifiers");
            adv(self);
            ty = make_ty_tag(union_specifier(self));
            goto set_match;
        case TK_ENUM:
            if (ty || had_ts) err("Invalid type specifiers");
            adv(self);
            ty = make_ty_tag(enum_specifier(self));
            goto set_match;
        case TK_IDENTIFIER:
            if (!ty && !had_ts)
                if ((ty = sema_findtypedef(&self->sema, tk_str(tk))))
                    break;
        default:
            if (!match)
                return ((void *)0);
            if (ty)
                return ty;
            return decode_ts(ts);
        }
        adv(self);
set_match:
        match = 1;
    }
}
static _Bool is_declaration_specifier(cc3_t *self, tk_t *tk)
{
    switch (tk->type) {
    case TK_TYPEDEF:
    case TK_EXTERN:
    case TK_STATIC:
    case TK_AUTO:
    case TK_REGISTER:
    case TK_CONST:
    case TK_RESTRICT:
    case TK_VOLATILE:
    case TK_INLINE:
    case TK_VOID:
    case TK_CHAR:
    case TK_SHORT:
    case TK_INT:
    case TK_LONG:
    case TK_FLOAT:
    case TK_DOUBLE:
    case TK_SIGNED:
    case TK_UNSIGNED:
    case TK_BOOL:
    case TK_COMPLEX:
    case TK_IMAGINARY:
    case TK_STRUCT:
    case TK_UNION:
    case TK_ENUM:
        return 1;
    case TK_IDENTIFIER:
        return sema_findtypedef(&self->sema, tk_str(tk)) != ((void *)0);
    default:
        return 0;
    }
}
static void type_qualifier_list(cc3_t *self)
{
    for (;;)
        switch (peek(self, 0)->type) {
        case TK_CONST:
        case TK_RESTRICT:
        case TK_VOLATILE:
            adv(self);
            break;
        default:
            return;
        }
}
static ty_t *declarator_suffixes(cc3_t *self, ty_t *ty)
{
    for (;;) {
        if (maybe_want(self, TK_LSQ)) {
            type_qualifier_list(self);
            maybe_want(self, TK_STATIC);
            type_qualifier_list(self);
            int cnt = -1;
            if (!maybe_want(self, TK_RSQ)) {
                cnt = constant_expression(self);
                want(self, TK_RSQ);
            }
            ty = make_array(ty, cnt);
            continue;
        }
        if (maybe_want(self, TK_LPAREN)) {
            sema_enter(&self->sema);
            ty_t *param_tys = ((void *)0), **tail = &param_tys;
            _Bool var = 0;
            if (peek(self, 0)->type == TK_VOID && peek(self, 1)->type == TK_RPAREN) {
                adv(self);
                adv(self);
            } else {
                for (;;) {
                    int sc;
                    if (!(*tail = declaration_specifiers(self, &sc)))
                        break;
                    char *name;
                    *tail = make_param(declarator(self, *tail, 1, &name));
                    if (name)
                        (*tail)->sym = sema_declare(&self->sema, sc, clone_ty(*tail), name);
                    if (!maybe_want(self, TK_COMMA))
                        break;
                    if (maybe_want(self, TK_ELLIPSIS)) {
                        var = 1;
                        break;
                    }
                    tail = &(*tail)->next;
                }
                want(self, TK_RPAREN);
            }
            ty = make_function(ty, param_tys, var);
            ty->function.scope = sema_pop(&self->sema);
            continue;
        }
        return ty;
    }
}
ty_t *declarator(cc3_t *self, ty_t *ty, _Bool allow_abstract,
    char **out_name)
{
    while (maybe_want(self, TK_MUL)) {
        type_qualifier_list(self);
        ty = make_pointer(ty);
    }
    if (maybe_want(self, TK_LPAREN)) {
        ty_t *dummy = calloc(1, sizeof *dummy);
        ty_t *result = declarator(self, dummy, allow_abstract, out_name);
        want(self, TK_RPAREN);
        ty = declarator_suffixes(self, ty);
        *dummy = *ty;
        free(ty);
        return result;
    }
    tk_t *tk;
    if (allow_abstract) {
        if ((tk = maybe_want(self, TK_IDENTIFIER)))
            *out_name = strdup(tk_str(tk));
        else
            *out_name = ((void *)0);
    } else {
        tk = want(self, TK_IDENTIFIER);
        *out_name = strdup(tk_str(tk));
    }
    return declarator_suffixes(self, ty);
}
static ty_t *type_name(cc3_t *self)
{
    int sc;
    ty_t *ty = declaration_specifiers(self, &sc);
    if (!ty)
        err("Expected type name");
    if (sc != -1)
        err("No storage class allowed");
    char *name;
    ty = declarator(self, ty, 1, &name);
    if (name)
        err("Only abstract declarators are allowed");
    return ty;
}
static inline void append_stmt(stmt_t ***tail, stmt_t *stmt)
{
    **tail = stmt;
    *tail = &(**tail)->next;
}
static _Bool block_scope_declaration(cc3_t *self, stmt_t ***tail)
{
    int sc;
    ty_t *base_ty = declaration_specifiers(self, &sc);
    if (!base_ty)
        return 0;
    if (!maybe_want(self, TK_SEMICOLON)) {
        do {
            char *name;
            ty_t *ty = declarator(self, clone_ty(base_ty), 0, &name);
            sym_t *sym = sema_declare(&self->sema, sc, ty, name);
            if (maybe_want(self, TK_AS)) {
                expr_t *expr = initializer(self);
                if (sym->offset != -1) {
                    stmt_t *stmt = make_stmt(STMT_INIT);
                    stmt->init.ty = clone_ty(sym->ty);
                    stmt->init.offset = sym->offset;
                    stmt->arg1 = expr;
                    append_stmt(tail, stmt);
                } else {
                    ((void) sizeof ((0) ? 1 : 0), ({ if (0) ; else __assert_fail ("0", "parse.c", 1067, __func__); }));
                }
            }
        } while (maybe_want(self, TK_COMMA));
        want(self, TK_SEMICOLON);
    }
    free_ty(base_ty);
    return 1;
}
static void statement(cc3_t *self, stmt_t ***tail);
static inline void block_item_list(cc3_t *self, stmt_t ***tail)
{
    while (!maybe_want(self, TK_RCURLY))
        if (!block_scope_declaration(self, tail))
            statement(self, tail);
}
static inline stmt_t *read_stmt(cc3_t *self)
{
    stmt_t *head = ((void *)0), **tail = &head;
    statement(self, &tail);
    return head;
}
void statement(cc3_t *self, stmt_t ***tail)
{
    tk_t *tk;
    for (;;) {
        if ((tk = peek(self, 0))->type == TK_IDENTIFIER
                && peek(self, 1)->type == TK_COLON) {
            stmt_t *stmt = make_stmt(STMT_LABEL);
            stmt->label = strdup(tk_str(tk));
            adv(self);
            adv(self);
            append_stmt(tail, stmt);
        } else if (maybe_want(self, TK_CASE)) {
            stmt_t *stmt = make_stmt(STMT_CASE);
            stmt->case_val = constant_expression(self);
            want(self, TK_COLON);
            append_stmt(tail, stmt);
        } else if (maybe_want(self, TK_DEFAULT)) {
            want(self, TK_COLON);
            append_stmt(tail, make_stmt(STMT_DEFAULT));
        } else {
            break;
        }
    }
    if (maybe_want(self, TK_LCURLY)) {
        sema_enter(&self->sema);
        block_item_list(self, tail);
        sema_exit(&self->sema);
        return;
    }
    if (maybe_want(self, TK_IF)) {
        stmt_t *stmt = make_stmt(STMT_IF);
        want(self, TK_LPAREN);
        stmt->arg1 = expression(self);
        want(self, TK_RPAREN);
        stmt->body1 = read_stmt(self);
        if (maybe_want(self, TK_ELSE))
            stmt->body2 = read_stmt(self);
        append_stmt(tail, stmt);
        return;
    }
    if (maybe_want(self, TK_SWITCH)) {
        stmt_t *stmt = make_stmt(STMT_SWITCH);
        want(self, TK_LPAREN);
        stmt->arg1 = expression(self);
        want(self, TK_RPAREN);
        stmt->body1 = read_stmt(self);
        append_stmt(tail, stmt);
        return;
    }
    if (maybe_want(self, TK_WHILE)) {
        stmt_t *stmt = make_stmt(STMT_WHILE);
        want(self, TK_LPAREN);
        stmt->arg1 = expression(self);
        want(self, TK_RPAREN);
        stmt->body1 = read_stmt(self);
        append_stmt(tail, stmt);
        return;
    }
    if (maybe_want(self, TK_DO)) {
        stmt_t *stmt = make_stmt(STMT_DO);
        stmt->body1 = read_stmt(self);
        want(self, TK_WHILE);
        want(self, TK_LPAREN);
        stmt->arg1 = expression(self);
        want(self, TK_RPAREN);
        want(self, TK_SEMICOLON);
        append_stmt(tail, stmt);
        return;
    }
    if (maybe_want(self, TK_FOR)) {
        stmt_t *stmt = make_stmt(STMT_FOR);
        want(self, TK_LPAREN);
        sema_enter(&self->sema);
        if (!block_scope_declaration(self, tail))
            if (!maybe_want(self, TK_SEMICOLON)) {
                stmt->arg1 = expression(self);
                want(self, TK_SEMICOLON);
            }
        if (!maybe_want(self, TK_SEMICOLON)) {
            stmt->arg2 = expression(self);
            want(self, TK_SEMICOLON);
        }
        if (!maybe_want(self, TK_RPAREN)) {
            stmt->arg3 = expression(self);
            want(self, TK_RPAREN);
        }
        stmt->body1 = read_stmt(self);
        sema_exit(&self->sema);
        append_stmt(tail, stmt);
        return;
    }
    if (maybe_want(self, TK_GOTO)) {
        stmt_t *stmt = make_stmt(STMT_GOTO);
        stmt->label = strdup(tk_str(want(self, TK_IDENTIFIER)));
        want(self, TK_SEMICOLON);
        append_stmt(tail, stmt);
        return;
    }
    if (maybe_want(self, TK_CONTINUE)) {
        want(self, TK_SEMICOLON);
        append_stmt(tail, make_stmt(STMT_CONTINUE));
        return;
    }
    if (maybe_want(self, TK_BREAK)) {
        want(self, TK_SEMICOLON);
        append_stmt(tail, make_stmt(STMT_BREAK));
        return;
    }
    if (maybe_want(self, TK_RETURN)) {
        stmt_t *stmt = make_stmt(STMT_RETURN);
        if (!maybe_want(self, TK_SEMICOLON)) {
            stmt->arg1 = expression(self);
            want(self, TK_SEMICOLON);
        }
        append_stmt(tail, stmt);
        return;
    }
    if (!maybe_want(self, TK_SEMICOLON)) {
        stmt_t *stmt = make_stmt(STMT_EVAL);
        stmt->arg1 = expression(self);
        want(self, TK_SEMICOLON);
        append_stmt(tail, stmt);
    }
}
static stmt_t *read_block(cc3_t *self)
{
    stmt_t *head = ((void *)0), **tail = &head;
    block_item_list(self, &tail);
    return head;
}
void parse(cc3_t *self)
{
    int sc;
    ty_t *base_ty;
    while ((base_ty = declaration_specifiers(self, &sc))) {
        if (!maybe_want(self, TK_SEMICOLON)) {
            char *name;
            ty_t *ty = declarator(self, clone_ty(base_ty), 0, &name);
            sym_t *sym = sema_declare(&self->sema, sc, ty, name);
            if (maybe_want(self, TK_LCURLY)) {
                if (ty->kind != TY_FUNCTION)
                    err("Function body after non-function declaration");
                sym->had_def = 1;
                self->sema.func_name = sym->name;
                self->sema.offset = 0;
                for (ty_t *param = sym->ty->function.param_tys; param; param = param->next) {
                    if (!param->sym)
                        err("Unnamed parameters are not allowed in function definitions");
                    self->sema.offset = align(self->sema.offset, ty_align(param));
                    param->sym->offset = self->sema.offset;
                    self->sema.offset += ty_size(param);
                }
                sema_push(&self->sema, ty->function.scope);
                stmt_t *body = read_block(self);
                sema_exit(&self->sema);
                gen_func(&self->gen, sym, self->sema.offset, body);
                free_ty(base_ty);
                continue;
            }
            if (maybe_want(self, TK_AS))
                initializer(self);
            while (maybe_want(self, TK_COMMA)) {
                char *name;
                ty_t *ty = declarator(self, clone_ty(base_ty), 0, &name);
                sema_declare(&self->sema, sc, ty, name);
                if (maybe_want(self, TK_AS))
                    initializer(self);
            }
            want(self, TK_SEMICOLON);
        }
        free_ty(base_ty);
    }
    want(self, TK_EOF);
}
