#ifndef __STDIO_H__
#define __STDIO_H__

#include <runtime/lib.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef NULL
#define NULL 0
#endif

#ifndef EOF
#define EOF -1
#endif

#ifndef BUFSIZ
#define BUFSIZ 512
#endif

#define SEEK_SET	0	/* Seek from beginning of file.  */
#define SEEK_CUR	1	/* Seek from current position.  */
#define SEEK_END	2	/* Seek from end of file.  */

int sscanf (const char *buf, const char *fmt, ...);
int snprintf (char *buf, int size, const char *fmt, ...);
int vsnprintf (char *buf, int size, const char *fmt, va_list args);

#define FILE			int
#define fpos_t			long
#define stdin			0
#define stdout			0
#define stderr			0

static inline FILE *fopen (const char *path, const char *mode)
	{ return 0; }

static inline FILE *fdopen (int fildes, const char *mode)
	{ return 0; }

static inline FILE *freopen (const char *path, const char *mode, FILE *stream)
	{ return 0; }

static inline int fclose (FILE *file)
	{ return 0; }

static inline int fseek (FILE *stream, long offset, int whence)
	{ return -1; }

static inline long ftell (FILE *stream)
	{ return 0L; }

static inline void rewind (FILE *stream)
	{}

static inline int fgetpos (FILE *stream, fpos_t *pos)
	{ return -1; }

static inline int fsetpos (FILE *stream, fpos_t *pos)
	{ return -1; }

static inline size_t fread (void *ptr, size_t size, size_t nmemb, FILE *stream)
	{ return 0; }

static inline size_t fwrite (const void *ptr, size_t size, size_t nmemb,
    FILE *stream)
	{ return 0; }

static inline void clearerr (FILE *stream)
	{}

static inline int feof (FILE *stream)
	{ return 1; }

static inline int ferror (FILE *stream)
	{ return 0; }

static inline int fileno (FILE *stream)
	{ return 0; }

static inline int fflush (FILE *stream)
	{ return 0; }

static inline void setbuf (FILE *stream, char *buf)
	{}

static inline void setbuffer (FILE *stream, char *buf, size_t size)
	{}

static inline void setlinebuf (FILE *stream)
	{}

static inline int setvbuf (FILE *stream, char *buf, int mode, size_t size)
	{ return 0; }

static inline int fputc (int c, FILE *stream)
	{ return c; }

static inline int fputs (const char *s, FILE *stream)
	{ return 0; }

static inline int putc (int c, FILE *stream)
	{ return c; }

static inline int putchar (int c)
	{ return c; }

static inline int puts (const char *s)
	{ return 0; }

static inline int fgetc (FILE *stream)
	{ return -1; }

static inline char *fgets (char *s, int size, FILE *stream)
	{ return 0; }

static inline int getc (FILE *stream)
	{ return -1; }

static inline int getchar (void)
	{ return -1; }

static inline char *gets (char *s)
	{ return 0; }

static inline int ungetc (int c, FILE *stream)
	{ return c; }

static inline int vfprintf (FILE *stream, const char *format, va_list ap)
	{ return 0; }

static inline int fscanf (FILE *fd, const char *fmt, ...)
	{ return 0; }

static inline int fprintf (FILE *fd, const char *fmt, ...)
{
	va_list args;
	int err;

	va_start (args, fmt);
	err = debug_vprintf (fmt, args);
	va_end (args);
	return err;
}

static inline int printf (const char *fmt, ...)
{
	va_list args;
	int err;

	va_start (args, fmt);
	err = debug_vprintf (fmt, args);
	va_end (args);
	return err;
}

#ifdef __cplusplus
}
#endif

#endif
