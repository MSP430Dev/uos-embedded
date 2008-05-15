// Line-oriented file access package.

# include "textbuf.h"
# include "extern.h"

# define MemCheckIndex(array, type, bound, quant, index)\
	if ((index) >= (bound)) (array) = (type) brealloc ((void *) (array),\
		(int) ((bound) += (quant)) * (int) sizeof (*(array)))

const int RDBUFSZ       = 512;
const int MAXLINE       = 1024; // maximum length of unbroken line

const char BAKSUFFIX [] = ".b";

static char mask [8] = { 1, 2, 4, 8, 0x10, 0x20, 0x40, 0x80, };
static char tfilepattern [] = "/tmp/recXXXXXX";

static int lenbuf;
static char *scanbuf, *pbuf;
static char *tfilename;
static int eoln;

static void *brealloc (void *ptr, int size)
{
	// realloc buffer to new size, retaining contents

	if (size == 0) {
		if (ptr)
			delete ((char*) ptr);
		return (0);
	}
	void *nptr = new char [size];
	if (ptr) {
		memcpy (nptr, ptr, size);
		delete ((char*) ptr);
	}
	return (nptr);
}

static int ffcopy (int from, int to)
{
	char buf [512], *p;
	int n, k;

	for (;;) {
		n = read (from, buf, sizeof (buf));
		if (n < 0) {
			xError ("Cannot read file");
			return (-1);
		}
		if (n == 0)
			return (0);
		p = buf;
		while (n > 0) {
			k = write (to, p, (unsigned) n);
			if (k <= 0) {
				xError ("Cannot write file");
				return (-1);
			}
			n -= k;
			p += k;
		}
	}
}

void TextBuffer::TempFree (long seek, int len)  // mark temp space free
{
	int i;

	len = (len + TEMPCELL - 1) / TEMPCELL;
	seek /= TEMPCELL;
	for (i=0; i<len; ++i, ++seek)
		tmap [seek>>3] &= ~mask [seek & 7];
}

long TextBuffer::TempBusy (int len)             // search & mark temp space busy
{
	int i, n;
	long seek;

	len = (len + TEMPCELL - 1) / TEMPCELL;
	for (seek=0; seek<MAXTEMP; ++seek) {
		for (n=0; n<len; ++n)
			if (tmap [seek>>3] & mask [seek & 7])
				break;
		if (n >= len)
			break;
	}
	if (seek >= MAXTEMP) {
		xError ("No temp space");
		return (0);
	}
	for (n=seek, i=0; i<len; ++i, ++n)
		tmap [n>>3] |= mask [n & 7];
	return (seek * TEMPCELL);
}

long TextBuffer::TempSave (char *str, int len)  // save string in temp file, return seek
{
	long seek;

	if (! len)
		return (0);
	seek = TempBusy (len);
	if (lseek (tfd, seek, 0) < 0)
		xError ("Cannot lseek on writing temp file");
	if (write (tfd, str, (unsigned) len) != len)
		xError ("Cannot write temporary file");
	return (seek);
}

static int scanline (int fd)
{
	int len;

	len = 0;
	eoln = 0;
	for (;;) {
		if (pbuf >= &scanbuf[lenbuf]) {
			lenbuf = read (fd, pbuf = scanbuf, (unsigned) RDBUFSZ);
			if (lenbuf <= 0)
				return (len ? len : -1);
		}
		if (*pbuf++ == '\n') {
			eoln = 1;
			return (len);
		}
		if (++len >= MAXLINE)
			return (len);
	}
}

TextBuffer::TextBuffer (int f, int wmode)
{
	TextLineIndex *x;
	int i;

	fd = f;
	bakfd = -1;
	if (wmode) {
		if (! tfilename)
			tfilename = tfilepattern;
		tfd = creat (tfilename, 0600);
		if (tfd < 0) {
			xError ("Cannot open temporary file");
			return;
		}
		close (tfd);
		tfd = open (tfilename, 2);
		if (tfd < 0) {
			xError ("Cannot reopen temporary file");
			return;
		}
		unlink (tfilename);
	} else
		tfd = -1;

	for (i=0; i<TEMPSZ; ++i)
		tmap [i] = 0;

	for (i=0; i<POOLSZ; ++i)
		map[i].busy = 0;

	lseek (fd, 0L, 0);
	broken = 0;
	length = 0;
	lindex = 0;
	nindex = 0;
	size = 0;
	scanbuf = new char [RDBUFSZ];
	pbuf = scanbuf;
	lenbuf = 0;
	for (;;) {
		MemCheckIndex (lindex, TextLineIndex *, nindex, QUANT, length+1);
		x = &lindex[length];
		x->seek = size;                 // store seek of current line
		x->poolindex = NOINDEX;         // clear pool index of current line
		x->flags = 0;                   // clear flags
		x->len = scanline (fd);         // scan next line
		if (length >= MAXLEN) {
			xError ("File is too long - truncated");
			break;
		}
		if (x->len < 0)
			break;
		size += x->len;                 // increment seek by length of line
		if (eoln)
			++size;                 // \n at end of line
		else {
			x->flags |= XNOEOLN;    // no end of line
			++broken;
		}
		++length;
	}
	delete scanbuf;
}

TextBuffer::~TextBuffer ()
{
	int i;

	for (i=0; i<POOLSZ; ++i)
		if (map[i].busy) {
			if (pool[i].len)
				delete pool[i].ptr;
			lindex[map[i].index].poolindex = NOINDEX;
			map[i].busy = 0;
		}
	delete lindex;
	if (tfd >= 0)
		close (tfd);
	if (bakfd >= 0)
		close (bakfd);
}

int TextBuffer::Save (char *filename)
{
	int i, newfd;
	TextLine *p;
	char bak [40];

	if (bakfd < 0) {
		strcpy (bak, filename);
		strcat (bak, BAKSUFFIX);
		bakfd = creat (bak, 0600);
		if (bakfd < 0) {
			xError ("Cannot create %s", bak);
			return (-1);
		}
		lseek (fd, 0L, 0);
		if (ffcopy (fd, bakfd) < 0) {
			close (bakfd);
			bakfd = -1;
			unlink (bak);
			return (-1);
		}
		close (bakfd);
		bakfd = open (bak, 0);
		if (bakfd < 0) {
			xError ("Cannot open %s", bak);
			unlink (bak);
			return (-1);
		}
		close (fd);
		fd = bakfd;
		bakfd = -1;
	}
	newfd = creat (filename, 0664);
	if (newfd < 0) {
		xError ("Cannot create %s", filename);
		unlink (bak);
		return (-1);
	}
	for (i=0; i<length; ++i) {
		p = Get (i);
		if (p->len)
			write (newfd, p->ptr, (unsigned) p->len);
		write (newfd, "\n", 1);
	}
	close (newfd);
	return (0);
}

void TextBuffer::Break ()               // break too long lines
{
	int i;
	TextLineIndex *x;

	x = lindex;
	for (i=0; i<length; ++i, ++x)
		x->flags &= ~XNOEOLN;
}

TextLine::TextLine (int fd, long seek, int len) // read line from file
{
	int l, n;
	char *s;

	len = len;
	mod = 0;
	if (! len) {
		ptr = "";
		return;
	}
	ptr = new char [len];
	if (lseek (fd, seek, 0) < 0)
		xError ("Cannot lseek on reading");
	for (l=len, s=ptr; l>0; l-=n, s+=n) {
		n = read (fd, s, (unsigned) l);
		if (n <= 0) {
			xError ("Cannot read line");
			if (len)
				delete ptr;
			len = 0;
			ptr = "";
			return;
		}
	}
}

int TextBuffer::FreeLine ()
{
	TextBufferMap *m;
	TextLineIndex *x;
	TextLine *l;
	int mintime, minindex;

	// find free place in pool

	for (m=map; m<map+POOLSZ; ++m)
		if (! m->busy)
			return (m - map);

	// pool is full; find the oldest line

	mintime = map[0].time;
	minindex = 0;
	for (m=map; m<map+POOLSZ; ++m)
		if (m->time < mintime)
			minindex = m - map;
	m = &map[minindex];
	l = &pool[minindex];
	x = &lindex[m->index];

	// remove line from pool

	if (l->mod) {           // line is modified, save it in temp file
		if ((x->flags & XTEMP) && l->oldlen)
			TempFree (x->seek, l->oldlen);
		x->seek = TempSave (l->ptr, l->len);
		x->len = l->len;
		x->flags |= XTEMP;
	}
	if (l->len)
		delete l->ptr;
	x->poolindex = NOINDEX;
	m->busy = 0;
	return (minindex);
}

TextLine *TextBuffer::Get (int n)
{
	TextLineIndex *x;
	TextBufferMap *m;
	TextLine *p;
	static long timecount = 1;              // time stamp

	if (n < 0 || n >= length)
		return (0);
	x = &lindex[n];
	if (x->poolindex != NOINDEX) {          // line is in cache
		map[x->poolindex].time = ++timecount;
		return (&pool[x->poolindex]);
	}
	x->poolindex = FreeLine ();             // get free pool index
	p = &pool[x->poolindex];
	m = &map[x->poolindex];
	m->time = ++timecount;
	m->index = n;
	m->busy = 1;

	// read line from file

	*p = TextLine ((x->flags & XTEMP) ? tfd : fd, x->seek, x->len);
	p->noeoln = (x->flags & XNOEOLN) != 0;
	return (p);
}

void TextLine::Put (int newlen)
{
	if (! mod)
		oldlen = len;
	mod = 1;
	len = newlen;
}

void TextBuffer::DeleteChar (int line, int off)
{
	TextLine *p;
	char *s;

	if (! (p = Get (line)))
		return;
	if (p->len <= off)
		return;
	s = new char [p->len - 1];
	if (off)
		memcpy (s, p->ptr, off);
	if (off <= p->len-1)
		memcpy (s+off, p->ptr+off+1, p->len-off-1);
	delete p->ptr;
	p->ptr = s;
	p->Put (p->len - 1);
}

void TextBuffer::InsertChar (int line, int off, int sym)
{
	TextLine *p;
	char *s;

	if (! (p = Get (line)))
		return;
	s = new char [p->len + 1];
	if (off)
		memcpy (s, p->ptr, off);
	s [off] = sym;
	if (off < p->len)
		memcpy (s+off+1, p->ptr+off, p->len-off);
	if (p->len)
		delete p->ptr;
	p->ptr = s;
	p->Put (p->len + 1);
}

void TextBuffer::InsertLine (int n)
{
	TextLineIndex *x, *i;
	TextBufferMap *m;
	int k;

	if (n<0 || n>length)
		return;
	++length;
	MemCheckIndex (lindex, TextLineIndex *, nindex, QUANT, length+1);
	i = &lindex[n];
	for (x= &lindex[length-1]; x>i; --x)
		x[0] = x[-1];
	i->seek = 0;
	i->len = 0;
	i->poolindex = NOINDEX;
	i->flags = XTEMP;
	m = map;
	for (k=0; k<POOLSZ; ++k, ++m)
		if (m->index >= n)
			++m->index;
}

void TextBuffer::DeleteLine (int n)
{
	TextLineIndex *x, *i;
	TextBufferMap *m;
	TextLine *l;
	int k;

	if (n<0 || n>=length)
		return;
	x = &lindex[n];
	m = map;
	if (x->poolindex != NOINDEX) {          // exclude line from pool
		l = &pool[x->poolindex];
		if (x->flags & XTEMP)
			if (l->mod) {
				if (l->oldlen)
					TempFree (x->seek, (long) l->oldlen);
			} else if (l->len)
				TempFree (x->seek, l->len);
		if (l->len)
			delete l->ptr;
		m[x->poolindex].busy = 0;
	}
	for (k=0; k<POOLSZ; ++k, ++m)
		if (m->index > n)
			--m->index;
	i = &lindex[length-1];
	for (x= &lindex[n]; x<i; ++x)
		x[0] = x[1];
	--length;
}
