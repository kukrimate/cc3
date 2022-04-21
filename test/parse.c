
extern void __assert_fail (const char *__assertion, const char *__file,
      unsigned int __line, const char *__function)
     ;
extern void __assert_perror_fail (int __errnum, const char *__file,
      unsigned int __line, const char *__function)
     ;
extern void __assert (const char *__assertion, const char *__file, int __line)
     ;

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
struct tk_buf {
    int type;
    int intval;
    char strval[4096];
};
const char *tk_str(int type);
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
    TY_STRUCT,
    TY_POINTER,
    TY_ARRAY,
    TY_FUNCTION,
};
struct member {
    struct member *next;
    struct ty *ty;
    const char *name;
};
struct ty {
    struct ty *next;
    int kind;
    union {
        struct {
            struct member *members;
        } stru;
        struct {
            struct ty *base_ty;
        } pointer;
        struct {
            struct ty *elem_ty;
            int cnt;
        } array;
        struct {
            struct ty *ret_ty;
            struct ty *param_tys;
            _Bool var;
        } function;
    };
};
struct ty *make_ty(int kind);
struct ty *make_pointer(struct ty *base_ty);
struct ty *make_array(struct ty *elem_ty, int cnt);
struct ty *make_function(struct ty *ret_ty, struct ty *param_tys, _Bool var);
void print_ty(struct ty *ty);
struct sym {
    struct sym *next;
    int sc;
    struct ty *ty;
    const char *name;
};
struct scope {
    struct scope *parent;
    struct sym *syms;
};
struct cc3 {
    FILE *fp;
    char buf[8192], *cur, *end;
    struct tk_buf tk_buf[2];
    int tk_pos, tk_cnt;
    struct scope *cur_scope;
};
void cc3_init(struct cc3 *self, FILE *fp);
struct tk_buf *lex_tok(struct cc3 *self, int i);
void lex_adv(struct cc3 *self);
void parse(struct cc3 *self);
void sema_enter(struct cc3 *self);
void sema_exit(struct cc3 *self);
void sema_declare(struct cc3 *self, int sc, struct ty *ty, const char *name);
 static void err(const char *fmt, ...)
{
    fputs("Parse error: ", stderr);
    va_list ap;
    __builtin_va_start(ap,fmt);
    vfprintf(stderr, fmt, ap);
    __builtin_va_end(ap);
    fputc('\n', stderr);
    abort();
}
static struct tk_buf *want(struct cc3 *self, int type)
{
    struct tk_buf *tk = lex_tok(self, 0);
    if (tk->type != type)
        err("Unexpected token %s expected %s", tk_str(tk->type), tk_str(type));
    lex_adv(self);
    return tk;
}
static struct tk_buf *maybe_want(struct cc3 *self, int type)
{
    struct tk_buf *tk = lex_tok(self, 0);
    if (tk->type == type) {
        lex_adv(self);
        return tk;
    }
    return ((void *)0);
}
static void primary_expression(struct cc3 *self);
static void postfix_expression(struct cc3 *self);
static void unary_expression(struct cc3 *self);
static void cast_expression(struct cc3 *self);
static void multiplicative_expression(struct cc3 *self);
static void additive_expression(struct cc3 *self);
static void shift_expression(struct cc3 *self);
static void relational_expression(struct cc3 *self);
static void equality_expression(struct cc3 *self);
static void and_expression(struct cc3 *self);
static void xor_expression(struct cc3 *self);
static void or_expression(struct cc3 *self);
static void land_expression(struct cc3 *self);
static void lor_expression(struct cc3 *self);
static void conditional_expression(struct cc3 *self);
static void assignment_expression(struct cc3 *self);
static void expression(struct cc3 *self);
static void constant_expression(struct cc3 *self);
void primary_expression(struct cc3 *self)
{
    struct tk_buf *tk = lex_tok(self, 0);
    switch (tk->type) {
    case TK_IDENTIFIER:
    case TK_CONSTANT:
    case TK_STR_LIT:
        lex_adv(self);
        break;
    case TK_LPAREN:
        lex_adv(self);
        expression(self);
        want(self, TK_RPAREN);
        break;
    default:
        err("Invalid primary expression %s", tk_str(tk->type));
    }
}
void postfix_expression(struct cc3 *self)
{
    primary_expression(self);
    for (;;) {
        if (maybe_want(self, TK_LSQ)) {
            expression(self);
            want(self, TK_RSQ);
            continue;
        }
        if (maybe_want(self, TK_LPAREN)) {
            if (maybe_want(self, TK_RPAREN))
                continue;
            do
                assignment_expression(self);
            while (maybe_want(self, TK_COMMA));
            want(self, TK_RPAREN);
            continue;
        }
        if (maybe_want(self, TK_DOT)) {
            want(self, TK_IDENTIFIER);
            continue;
        }
        if (maybe_want(self, TK_ARROW)) {
            want(self, TK_IDENTIFIER);
            continue;
        }
        if (maybe_want(self, TK_INCR))
            ;
        if (maybe_want(self, TK_DECR))
            ;
        break;
    }
}
void unary_expression(struct cc3 *self)
{
    if (maybe_want(self, TK_INCR))
        unary_expression(self);
    else if (maybe_want(self, TK_DECR))
        unary_expression(self);
    else if (maybe_want(self, TK_AND))
        cast_expression(self);
    else if (maybe_want(self, TK_MUL))
        cast_expression(self);
    else if (maybe_want(self, TK_ADD))
        cast_expression(self);
    else if (maybe_want(self, TK_SUB))
        cast_expression(self);
    else if (maybe_want(self, TK_NOT))
        cast_expression(self);
    else if (maybe_want(self, TK_LNOT))
        cast_expression(self);
    else
        postfix_expression(self);
}
void cast_expression(struct cc3 *self)
{
    unary_expression(self);
}
void multiplicative_expression(struct cc3 *self)
{
    cast_expression(self);
    for (;;)
        if (maybe_want(self, TK_MUL))
            cast_expression(self);
        else if (maybe_want(self, TK_DIV))
            cast_expression(self);
        else if (maybe_want(self, TK_MOD))
            cast_expression(self);
        else
            return;
}
void additive_expression(struct cc3 *self)
{
    multiplicative_expression(self);
    for (;;)
        if (maybe_want(self, TK_ADD))
            multiplicative_expression(self);
        else if (maybe_want(self, TK_SUB))
            multiplicative_expression(self);
        else
            return;
}
void shift_expression(struct cc3 *self)
{
    additive_expression(self);
    for (;;)
        if (maybe_want(self, TK_LSH))
            additive_expression(self);
        else if (maybe_want(self, TK_RSH))
            additive_expression(self);
        else
            return;
}
void relational_expression(struct cc3 *self)
{
    shift_expression(self);
    for (;;)
        if (maybe_want(self, TK_LT))
            shift_expression(self);
        else if (maybe_want(self, TK_GT))
            shift_expression(self);
        else if (maybe_want(self, TK_LE))
            shift_expression(self);
        else if (maybe_want(self, TK_GE))
            shift_expression(self);
        else
            return;
}
void equality_expression(struct cc3 *self)
{
    relational_expression(self);
    for (;;)
        if (maybe_want(self, TK_EQ))
            relational_expression(self);
        else if (maybe_want(self, TK_NE))
            relational_expression(self);
        else
            return;
}
void and_expression(struct cc3 *self)
{
    equality_expression(self);
    while (maybe_want(self, TK_AND))
        equality_expression(self);
}
void xor_expression(struct cc3 *self)
{
    and_expression(self);
    while (maybe_want(self, TK_XOR))
        and_expression(self);
}
void or_expression(struct cc3 *self)
{
    xor_expression(self);
    while (maybe_want(self, TK_OR))
        xor_expression(self);
}
void land_expression(struct cc3 *self)
{
    or_expression(self);
    while (maybe_want(self, TK_LAND))
        or_expression(self);
}
void lor_expression(struct cc3 *self)
{
    land_expression(self);
    while (maybe_want(self, TK_LOR))
        land_expression(self);
}
void conditional_expression(struct cc3 *self)
{
    lor_expression(self);
    if (!maybe_want(self, TK_COND))
        return;
    expression(self);
    want(self, TK_COLON);
    conditional_expression(self);
}
void assignment_expression(struct cc3 *self)
{
    conditional_expression(self);
    if (maybe_want(self, TK_AS))
        assignment_expression(self);
    else if (maybe_want(self, TK_MUL_AS))
        assignment_expression(self);
    else if (maybe_want(self, TK_DIV_AS))
        assignment_expression(self);
    else if (maybe_want(self, TK_MOD_AS))
        assignment_expression(self);
    else if (maybe_want(self, TK_ADD_AS))
        assignment_expression(self);
    else if (maybe_want(self, TK_SUB_AS))
        assignment_expression(self);
    else if (maybe_want(self, TK_LSH_AS))
        assignment_expression(self);
    else if (maybe_want(self, TK_RSH_AS))
        assignment_expression(self);
    else if (maybe_want(self, TK_AND_AS))
        assignment_expression(self);
    else if (maybe_want(self, TK_XOR_AS))
        assignment_expression(self);
    else if (maybe_want(self, TK_OR_AS))
        assignment_expression(self);
}
void expression(struct cc3 *self)
{
    assignment_expression(self);
    while (maybe_want(self, TK_COMMA))
        assignment_expression(self);
}
void constant_expression(struct cc3 *self)
{
    conditional_expression(self);
}
static struct ty *declaration_specifiers(struct cc3 *self);
static void struct_declaration_list(struct cc3 *self);
static void enumerator_list(struct cc3 *self);
static struct ty *declarator(struct cc3 *self, struct ty *ty,
    _Bool allow_abstract,
    const char **out_name);
static void type_qualifier_list(struct cc3 *self);
static void initializer(struct cc3 *self);
static void initializer_list(struct cc3 *self);
static void designation(struct cc3 *self);
enum {
    TS_VOID, TS_CHAR, TS_SHORT, TS_INT, TS_LONG, TS_FLOAT, TS_DOUBLE,
    TS_SIGNED, TS_UNSIGNED, TS_BOOL, TS_COMPLEX, TS_IMAGINARY, NUM_TS
};
static struct ty *decode_ts(int ts[static NUM_TS])
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
        { { [TS_SIGNED] = 1, [TS_SHORT] = 1, [TY_INT] = 1 }, TY_SHORT },
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
static struct ty *declaration_specifiers(struct cc3 *self)
{
    _Bool match = 0;
    int sc = -1;
    int ts[NUM_TS] = {0};
    for (;;) {
        struct tk_buf *tk = lex_tok(self, 0);
        switch (tk->type) {
        case TK_TYPEDEF:
        case TK_EXTERN:
        case TK_STATIC:
        case TK_AUTO:
        case TK_REGISTER:
            if (sc < 0)
                sc = tk->type;
            else
                err("Unexpected %s already had %s", tk_str(tk->type), tk_str(sc));
            break;
        case TK_VOID: ++ts[TS_VOID]; break;
        case TK_CHAR: ++ts[TS_CHAR]; break;
        case TK_SHORT: ++ts[TS_SHORT]; break;
        case TK_INT: ++ts[TS_INT]; break;
        case TK_LONG: ++ts[TS_LONG]; break;
        case TK_FLOAT: ++ts[TS_FLOAT]; break;
        case TK_DOUBLE: ++ts[TS_DOUBLE]; break;
        case TK_SIGNED: ++ts[TS_SIGNED]; break;
        case TK_UNSIGNED: ++ts[TS_UNSIGNED]; break;
        case TK_BOOL: ++ts[TS_BOOL]; break;
        case TK_COMPLEX: ++ts[TS_COMPLEX]; break;
        case TK_IMAGINARY: ++ts[TS_IMAGINARY]; break;
        case TK_STRUCT:
        case TK_UNION:
            lex_adv(self);
            if ((tk = maybe_want(self, TK_IDENTIFIER)))
                ;
            if (maybe_want(self, TK_LCURLY))
                struct_declaration_list(self);
            continue;
        case TK_ENUM:
            lex_adv(self);
            if ((tk = maybe_want(self, TK_IDENTIFIER)))
                ;
            if (maybe_want(self, TK_LCURLY))
                enumerator_list(self);
            continue;
        case TK_CONST:
        case TK_RESTRICT:
        case TK_VOLATILE:
            break;
        case TK_INLINE:
            break;
        default:
            if (!match)
                return ((void *)0);
            return decode_ts(ts);
        }
        lex_adv(self);
        match = 1;
    }
}
void struct_declaration_list(struct cc3 *self)
{
    do {
        struct ty *ty = declaration_specifiers(self);
        const char *name;
        if (!ty)
            err("Expected declaration in struct");
        do {
            if (maybe_want(self, TK_COLON)) {
                constant_expression(self);
                continue;
            }
            declarator(self, ty, 0, &name);
            if (maybe_want(self, TK_COLON))
                constant_expression(self);
        } while (!maybe_want(self, TK_SEMICOLON));
    } while (!maybe_want(self, TK_RCURLY));
}
void enumerator_list(struct cc3 *self)
{
    for (;;) {
        want(self, TK_IDENTIFIER);
        if (maybe_want(self, TK_AS))
            constant_expression(self);
        if (maybe_want(self, TK_COMMA)) {
            if (maybe_want(self, TK_RCURLY))
                return;
        } else {
            want(self, TK_RCURLY);
            return;
        }
    }
}
static struct ty *declarator_suffixes(struct cc3 *self, struct ty *ty)
{
    for (;;) {
        if (maybe_want(self, TK_LSQ)) {
            type_qualifier_list(self);
            maybe_want(self, TK_STATIC);
            type_qualifier_list(self);
            int cnt = -1;
            if (!maybe_want(self, TK_RSQ)) {
                          constant_expression(self);
                want(self, TK_RSQ);
            }
            ty = make_array(ty, cnt);
            continue;
        }
        if (maybe_want(self, TK_LPAREN)) {
            if (maybe_want(self, TK_RPAREN)) {
                ty = make_function(ty, ((void *)0), 1);
                continue;
            }
            if (maybe_want(self, TK_IDENTIFIER)) {
                while (maybe_want(self, TK_COMMA))
                    want(self, TK_IDENTIFIER);
                want(self, TK_RPAREN);
                ty = make_function(ty, ((void *)0), 1);
                continue;
            }
            sema_enter(self);
            struct ty *param_tys = ((void *)0), **cur_param_ty = &param_tys;
            _Bool var = 0;
            while ((*cur_param_ty = declaration_specifiers(self))) {
                const char *param_name;
                *cur_param_ty = declarator(self, *cur_param_ty, 1, &param_name);
                if (param_name)
                    sema_declare(self, 0, *cur_param_ty, param_name);
                if (maybe_want(self, TK_COMMA)) {
                    if (maybe_want(self, TK_ELLIPSIS)) {
                        var = 1;
                        break;
                    }
                } else {
                    break;
                }
                cur_param_ty = &(*cur_param_ty)->next;
            }
            want(self, TK_RPAREN);
            sema_exit(self);
            ty = make_function(ty, param_tys, var);
            continue;
        }
        return ty;
    }
}
struct ty *declarator(struct cc3 *self, struct ty *ty, _Bool allow_abstract,
    const char **out_name)
{
    while (maybe_want(self, TK_MUL)) {
        type_qualifier_list(self);
        ty = make_pointer(ty);
    }
    if (maybe_want(self, TK_LPAREN)) {
        struct ty *dummy = calloc(1, sizeof *dummy);
        struct ty *result = declarator(self, dummy, allow_abstract, out_name);
        want(self, TK_RPAREN);
        ty = declarator_suffixes(self, ty);
        *dummy = *ty;
        free(ty);
        return result;
    }
    struct tk_buf *tk;
    if (allow_abstract) {
        if ((tk = maybe_want(self, TK_IDENTIFIER)))
            *out_name = strdup(tk->strval);
        else
            *out_name = ((void *)0);
    } else {
        tk = want(self, TK_IDENTIFIER);
        *out_name = strdup(tk->strval);
    }
    return declarator_suffixes(self, ty);
}
void type_qualifier_list(struct cc3 *self)
{
    for (;;)
        switch (lex_tok(self, 0)->type) {
        case TK_CONST:
        case TK_RESTRICT:
        case TK_VOLATILE:
            lex_adv(self);
            break;
        default:
            return;
        }
}
void initializer(struct cc3 *self)
{
    if (maybe_want(self, TK_LCURLY))
        initializer_list(self);
    else
        assignment_expression(self);
}
void initializer_list(struct cc3 *self)
{
    for (;;) {
        designation(self);
        initializer(self);
        if (maybe_want(self, TK_COMMA)) {
            if (maybe_want(self, TK_RCURLY))
                return;
        } else {
            want(self, TK_RCURLY);
            return;
        }
    }
}
void designation(struct cc3 *self)
{
    _Bool match = 0;
    for (;;)
        if (maybe_want(self, TK_LSQ)) {
            constant_expression(self);
            want(self, TK_RSQ);
            match = 1;
        } else if (maybe_want(self, TK_DOT)) {
            want(self, TK_IDENTIFIER);
            match = 1;
        } else {
            goto end;
        }
end:
    if (match)
        want(self, TK_AS);
}
static _Bool declaration(struct cc3 *self)
{
    struct ty *base_ty = declaration_specifiers(self);
    if (!base_ty)
        return 0;
    if (!maybe_want(self, TK_SEMICOLON)) {
        do {
            const char *name;
            struct ty *ty = declarator(self, base_ty, 0, &name);
            sema_declare(self, 0, ty, name);
            if (maybe_want(self, TK_AS))
                initializer(self);
        } while (maybe_want(self, TK_COMMA));
        want(self, TK_SEMICOLON);
    }
    return 1;
}
static void statement(struct cc3 *self);
static void block_item_list(struct cc3 *self)
{
    sema_enter(self);
    while (!maybe_want(self, TK_RCURLY))
        if (!declaration(self))
            statement(self);
    sema_exit(self);
}
void statement(struct cc3 *self)
{
    if (lex_tok(self, 0)->type == TK_IDENTIFIER
            && lex_tok(self, 1)->type == TK_COLON) {
        lex_adv(self);
        lex_adv(self);
    } else if (maybe_want(self, TK_CASE)) {
        constant_expression(self);
        want(self, TK_COLON);
    } else if (maybe_want(self, TK_DEFAULT)) {
        want(self, TK_COLON);
    }
    if (maybe_want(self, TK_LCURLY)) {
        block_item_list(self);
        return;
    }
    if (maybe_want(self, TK_IF)) {
        want(self, TK_LPAREN);
        expression(self);
        want(self, TK_RPAREN);
        statement(self);
        if (maybe_want(self, TK_ELSE))
            statement(self);
        return;
    }
    if (maybe_want(self, TK_SWITCH)) {
        want(self, TK_LPAREN);
        expression(self);
        want(self, TK_RPAREN);
        statement(self);
        return;
    }
    if (maybe_want(self, TK_WHILE)) {
        want(self, TK_LPAREN);
        expression(self);
        want(self, TK_RPAREN);
        statement(self);
        return;
    }
    if (maybe_want(self, TK_DO)) {
        statement(self);
        want(self, TK_WHILE);
        want(self, TK_LPAREN);
        expression(self);
        want(self, TK_RPAREN);
        want(self, TK_SEMICOLON);
        return;
    }
    if (maybe_want(self, TK_FOR)) {
        want(self, TK_LPAREN);
        sema_enter(self);
        if (!declaration(self))
            if (!maybe_want(self, TK_SEMICOLON)) {
                expression(self);
                want(self, TK_SEMICOLON);
            }
        if (!maybe_want(self, TK_SEMICOLON)) {
            expression(self);
            want(self, TK_SEMICOLON);
        }
        if (!maybe_want(self, TK_RPAREN)) {
            expression(self);
            want(self, TK_RPAREN);
        }
        statement(self);
        sema_exit(self);
        return;
    }
    if (maybe_want(self, TK_GOTO)) {
        want(self, TK_IDENTIFIER);
        want(self, TK_SEMICOLON);
        return;
    }
    if (maybe_want(self, TK_CONTINUE)) {
        want(self, TK_SEMICOLON);
        return;
    }
    if (maybe_want(self, TK_BREAK)) {
        want(self, TK_SEMICOLON);
        return;
    }
    if (maybe_want(self, TK_RETURN)) {
        if (!maybe_want(self, TK_SEMICOLON)) {
            expression(self);
            want(self, TK_SEMICOLON);
        }
        return;
    }
    if (!maybe_want(self, TK_SEMICOLON)) {
        expression(self);
        want(self, TK_SEMICOLON);
    }
}
void parse(struct cc3 *self)
{
    struct ty *base_ty;
    while ((base_ty = declaration_specifiers(self))) {
        if (maybe_want(self, TK_SEMICOLON))
            continue;
        const char *name;
        struct ty *ty = declarator(self, base_ty, 0, &name);
        sema_declare(self, 0, ty, name);
        if (maybe_want(self, TK_LCURLY)) {
            block_item_list(self);
            continue;
        }
        if (maybe_want(self, TK_AS))
            initializer(self);
        while (maybe_want(self, TK_COMMA)) {
            const char *name;
            struct ty *ty = declarator(self, base_ty, 0, &name);
            sema_declare(self, 0, ty, name);
            if (maybe_want(self, TK_AS))
                initializer(self);
        }
        want(self, TK_SEMICOLON);
    }
}
