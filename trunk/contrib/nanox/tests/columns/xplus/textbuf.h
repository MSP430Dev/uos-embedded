// Line-oriented file access package.

class TextLine {                        // in core line descriptor
public:
	char            *ptr;           // pointer to string
	short           len;            // length of string

	void Put (int newlen);          // put line into buffer

private:
	short           oldlen;         // length before modification
	unsigned        mod :8;         // line is modified
	unsigned        noeoln :8;      // no end of line

	TextLine () {};
	TextLine (int fd, long seek, int len); // read line from file

	friend class TextBuffer;
};

class TextBuffer {
public:
	short           length;         // length of file in lines
	long            size;           // length of file in bytes

	// Initialize TextBuffer package for file fd,
	// for writing if mode != 0.

	TextBuffer (int fd, int wmode = 0);

	// Initialize TextBuffer package with string str.
	// Length of string is len.

	TextBuffer (char *str, int len);

	// Close TextBuffer discarding changes.

	~TextBuffer ();

	// Write out changes.
	// Returns 0 if ok or -1 if cannot write file.

	int Save (char *fnam);

	// Get line.

	TextLine *Get (int lnum);

	// Insert empty line before line #linenum.

	void InsertLine (int linenum);

	// Delete line.

	void DeleteLine (int linenum);

	// Insert char into line.

	void InsertChar (int line, int off, int sym);

	// Delete char from line.

	void DeleteChar (int line, int off);

	// If TextBuffer has broken lines, return 1, else 0.

	int Broken ();

	// Break too long lines.

	void Break ();

private:
	static const int SEARCHSZ	= 41;   // max size of search string
	static const int MAXTEMP	= 2048; // max of temp file size (in cells)
	static const int TEMPSZ		= ((MAXTEMP+7)/8); // temp table size (in bytes)
	static const int POOLSZ		= 50;   // number of lines in cache

	static const int XTEMP		= 1;    // line is in tmp file
	static const int XNOEOLN	= 2;    // no end of line

	static const int TEMPCELL	= 16;   // minimal size of temp space
	static const int QUANT		= 512;
	static const int MAXLEN		= 3000; // maximum length of file
	static const unsigned NOINDEX	= POOLSZ + 2;

	struct TextLineIndex {          // out of core line descriptor
		long    seek;           // seek in file
		short   len;            // length of line
		unsigned poolindex :8;  // index in pool or NOINDEX
		unsigned flags :8;      // is in tmp file
	};

	struct TextBufferMap {          // pool cell descriptor
		short   busy;           // cell busy
		short   index;          // index in lindex
		long    time;           // time of last access
	};

	TextLineIndex   *lindex;        // out of core line descriptors
	TextBufferMap   map [POOLSZ];   // line pool
	TextLine        pool [POOLSZ];  // in core line descriptors
	char            tmap [TEMPSZ];  // temp file map
	long            nindex;         // number of indexes malloc'ed
	short           fd;             // file descriptor
	short           bakfd;          // bak file descriptor
	short           tfd;            // temp file descriptor
	short           broken;         // there are broken lines

	void TempFree (long, int);      // mark temp space free
	long TempBusy (int);            // search & mark temp space busy
	long TempSave (char *, int);    // save string in temp file, return seek
	int FreeLine ();                // find free line in pool
};
