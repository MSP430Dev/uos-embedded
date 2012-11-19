/*
 * Convert BDF files to C source and/or Rockbox .fnt file format
 *
 * Copyright (c) 2002 by Greg Haerr <greg@censoft.com>
 *
 * What fun it is converting font data...
 *
 * 09/17/02	Version 1.0
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include FONTFILE

#define MAXHEIGHT	16
#define MAXNCHAR	0x500

/* Glyph image helper macros. */
#define	BITS_PER_WORD		(sizeof(unsigned short) * 8)
#define NIBBLES_PER_WORD	(BITS_PER_WORD/4)
#define WORDS(bits)		(((bits)+BITS_PER_WORD-1)/BITS_PER_WORD)
#define BYTES(bits)		(WORDS(bits) * sizeof(unsigned short))
#define	TEST_HIGH_BIT(w)	((w) >> (BITS_PER_WORD - 1) & 1)

/* builtin C-based proportional/fixed font structure */
/* based on The Microwindows Project http://microwindows.org */
typedef struct {
	int		maxwidth;		/* max width in pixels*/
	int 		height;			/* height in pixels*/
	int		ascent;			/* ascent (baseline) height*/
	int		descent;
	int		firstchar;		/* first character in bitmap*/
	int		size;			/* font size in glyphs*/
	unsigned short	bits [256 * MAXHEIGHT];	/* 16-bit right-padded bitmap data*/
	unsigned long	offset [MAXNCHAR];	/* offsets into bitmap data*/
	int 		have_encodetable;
	unsigned char 	width [MAXNCHAR];	/* character widths or NULL if fixed*/
	int 		proportional;
	int		defaultchar;		/* default char (not glyph index)*/
} font_t;

#define isprefix(buf,str)	(!strncmp(buf, str, strlen(str)))
#define	strequal(s1,s2)		(!strcmp(s1, s2))

const unsigned short cp1251_to_unicode [256] = {
	0x00,   0x01,   0x02,   0x03,   0x04,   0x05,   0x06,   0x07,
	0x08,   0x09,   0x0a,   0x0b,   0x0c,   0x0d,   0x0e,   0x0f,
	0x10,   0x11,   0x12,   0x13,   0x14,   0x15,   0x16,   0x17,
	0x18,   0x19,   0x1a,   0x1b,   0x1c,   0x1d,   0x1e,   0x1f,
	0x20,   0x21,   0x22,   0x23,   0x24,   0x25,   0x26,   0x27,
	0x28,   0x29,   0x2a,   0x2b,   0x2c,   0x2d,   0x2e,   0x2f,
	0x30,   0x31,   0x32,   0x33,   0x34,   0x35,   0x36,   0x37,
	0x38,   0x39,   0x3a,   0x3b,   0x3c,   0x3d,   0x3e,   0x3f,
	0x40,   0x41,   0x42,   0x43,   0x44,   0x45,   0x46,   0x47,
	0x48,   0x49,   0x4a,   0x4b,   0x4c,   0x4d,   0x4e,   0x4f,
	0x50,   0x51,   0x52,   0x53,   0x54,   0x55,   0x56,   0x57,
	0x58,   0x59,   0x5a,   0x5b,   0x5c,   0x5d,   0x5e,   0x5f,
	0x60,   0x61,   0x62,   0x63,   0x64,   0x65,   0x66,   0x67,
	0x68,   0x69,   0x6a,   0x6b,   0x6c,   0x6d,   0x6e,   0x6f,
	0x70,   0x71,   0x72,   0x73,   0x74,   0x75,   0x76,   0x77,
	0x78,   0x79,   0x7a,   0x7b,   0x7c,   0x7d,   0x7e,   0x7f,
	0x0402, 0x0403, 0x201a, 0x0453, 0x201e, 0x2026, 0x2020, 0x2021,
	0x20ac, 0x2030, 0x0409, 0x2039, 0x040a, 0x040c, 0x040b, 0x040f,
	0x0452, 0x2018, 0x2019, 0x201c, 0x201d, 0x2022, 0x2013, 0x2014,
	0x98,   0x2122, 0x0459, 0x203a, 0x045a, 0x045c, 0x045b, 0x045f,
	0xa0,   0x040e, 0x045e, 0x0408, 0xa4,   0x0490, 0xa6,   0xa7,
	0x0401, 0xa9,   0x0404, 0xab,   0xac,   0xad,   0xae,   0x0407,
	0xb0,   0xb1,   0x0406, 0x0456, 0x0491, 0xb5,   0xb6,   0xb7,
	0x0451, 0x2116, 0x0454, 0xbb,   0x0458, 0x0405, 0x0455, 0x0457,
	0x0410, 0x0411, 0x0412, 0x0413, 0x0414, 0x0415, 0x0416, 0x0417,
	0x0418, 0x0419, 0x041a, 0x041b, 0x041c, 0x041d, 0x041e, 0x041f,
	0x0420, 0x0421, 0x0422, 0x0423, 0x0424, 0x0425, 0x0426, 0x0427,
	0x0428, 0x0429, 0x042a, 0x042b, 0x042c, 0x042d, 0x042e, 0x042f,
	0x0430, 0x0431, 0x0432, 0x0433, 0x0434, 0x0435, 0x0436, 0x0437,
	0x0438, 0x0439, 0x043a, 0x043b, 0x043c, 0x043d, 0x043e, 0x043f,
	0x0440, 0x0441, 0x0442, 0x0443, 0x0444, 0x0445, 0x0446, 0x0447,
	0x0448, 0x0449, 0x044a, 0x044b, 0x044c, 0x044d, 0x044e, 0x044f,
};

/* build incore structure from font.c file*/
font_t *
read_font()
{
	font_t *pf;

	pf = (font_t *) calloc(1, sizeof(font_t));
	if (! pf)
		return 0;

	pf->firstchar = ' ';
	pf->defaultchar = 0x7f;
	pf->height = FONTNAME.height;
	pf->maxwidth = FONTNAME.width;
	pf->descent = 1;
	pf->ascent = pf->height - pf->descent;

	/* initially mark offsets as not used*/
	int i;
	for (i=0; i<MAXNCHAR; ++i)
		pf->offset[i] = -1;

	long ofs = 0;
	int ch;
	for (ch=' '; ch<=FONTNAME.count; ch++) {
		/* Skip nontext characters. */
		if (ch > 0x7f && ch < 0xc0)
			continue;

		int encoding = cp1251_to_unicode [ch];
		if (encoding < pf->firstchar)
			continue;
		if (pf->size <= encoding - pf->firstchar)
			pf->size = encoding - pf->firstchar + 1;

		/* set bits offset in encode map*/
		pf->offset [encoding - pf->firstchar] = ofs;
		pf->width [encoding - pf->firstchar] = pf->maxwidth;

		/* clear bitmap*/
		unsigned short *ch_bitmap = pf->bits + ofs;
		memset(ch_bitmap, 0, BYTES(pf->maxwidth) * pf->height);

		/* read bitmaps*/
		int k;
		for (k=0; k<pf->maxwidth; ++k) {
			unsigned value = FONTNAME.data [ch * pf->maxwidth + k];
			for (i=0; i<pf->height; ++i) {
				if (value >> i & 1)
					ch_bitmap [i] |= 0x8000 >> k;
			}
		}
		ofs += WORDS (pf->maxwidth) * pf->height;
	}

	/* change unused offset/width values to default char values*/
	for (i=0; i<pf->size; ++i) {
		int defchar = pf->defaultchar - pf->firstchar;

		if (pf->offset[i] == (unsigned long)-1) {
			pf->offset[i] = pf->offset[defchar];
			pf->width[i] = pf->width[defchar];
		}
	}

	/* determine whether font doesn't require encode table*/
	pf->have_encodetable = 0;
	long l = 0;
	for (i=0; i<pf->size; ++i) {
		if (pf->offset[i] != l) {
			pf->have_encodetable = 1;
			break;
		}
		l += WORDS(pf->width[i]) * pf->height;
	}

	/* determine whether font is fixed-width*/
	pf->proportional = 0;
	for (i=0; i<pf->size; ++i) {
		if (pf->width[i] != pf->maxwidth) {
			pf->proportional = 1;
			break;
		}
	}
	return pf;
}

/* generate C source from in-core font*/
int
gen_c_source(font_t *pf)
{
	FILE *ofp;
	int i;
	int did_defaultchar = 0;
	int did_syncmsg = 0;
	time_t t = time(0);
	unsigned short *ofs = pf->bits;
	char buf[256];
	char obuf[256];
	char hdr1[] = {
		"/* Generated by convfont on %s. */\n"
		"#include <gpanel/gpanel.h>\n"
		"\n"
		"/* Font information:\n"
		"   w x h: %dx%d\n"
		"   size: %d\n"
		"   ascent: %d\n"
		"   descent: %d\n"
		"   first char: %d (0x%02x)\n"
		"   last char: %d (0x%02x)\n"
		"   default char: %d (0x%02x)\n"
		"   proportional: %s\n"
		"*/\n"
		"\n"
		"/* Font character bitmap data. */\n"
		"static const unsigned short font_bits[] = {\n"
	};

	ofp = stdout;
	strcpy(buf, ctime(&t));
	buf[strlen(buf)-1] = 0;

	fprintf(ofp, hdr1, buf,
		pf->maxwidth, pf->height,
		pf->size,
		pf->ascent, pf->descent,
		pf->firstchar, pf->firstchar,
		pf->firstchar+pf->size-1, pf->firstchar+pf->size-1,
		pf->defaultchar, pf->defaultchar,
		pf->proportional ? "yes": "no");

	/* generate bitmaps*/
	for (i=0; i<pf->size; ++i) {
		int x;
		int bitcount = 0;
 		int width = pf->proportional ? pf->width[i] : pf->maxwidth;
		int height = pf->height;
		unsigned short *bits = pf->bits + (pf->have_encodetable ?
			pf->offset[i] : (pf->height * i));
		unsigned short bitvalue = 0;

		/*
		 * Generate bitmap bits only if not this index isn't
		 * the default character in encode map, or the default
		 * character hasn't been generated yet.
		 */
		if (pf->have_encodetable &&
		    (pf->offset[i] == pf->offset[pf->defaultchar-pf->firstchar])) {
			if (did_defaultchar)
				continue;
			did_defaultchar = 1;
		}

		fprintf(ofp, "\n/* Character %d (0x%02x):\n   width %d",
			i+pf->firstchar, i+pf->firstchar, width);

		fprintf(ofp, "\n   +");
		for (x=0; x<width; ++x) fprintf(ofp, "-");
		fprintf(ofp, "+\n");

		x = 0;
		while (height > 0) {
			if (x == 0) fprintf(ofp, "   |");

			if (bitcount <= 0) {
				bitcount = BITS_PER_WORD;
				bitvalue = *bits++;
			}

			fprintf(ofp, TEST_HIGH_BIT(bitvalue)? "*": " ");

			bitvalue <<= 1;
			--bitcount;
			if (++x == width) {
				fprintf(ofp, "|\n");
				--height;
				x = 0;
				bitcount = 0;
			}
		}
		fprintf(ofp, "   +");
		for (x=0; x<width; ++x) fprintf(ofp, "-");
		fprintf(ofp, "+ */\n");

		bits = pf->bits + (pf->have_encodetable ?
			pf->offset[i] : (pf->height * i));
		x = WORDS(width) * (pf->height);
		for (; x>0; --x) {
			fprintf(ofp, "0x%04x,\n", *bits);
			if (*bits++ != *ofs++ && !did_syncmsg) {
				fprintf(stderr, "Warning: found encoding values in non-sorted order (not an error).\n");
				did_syncmsg = 1;
			}
		}
	}
	fprintf(ofp, 	"};\n\n");

	if (pf->have_encodetable) {
		long offset, default_offset;

		/* output offset table*/
		fprintf(ofp, "/* Character->glyph mapping. */\n"
			"static const unsigned short font_offset[] = {\n");
		offset = 0;
		did_defaultchar = 0;
		default_offset = 0;
		for (i=0; i<pf->size; ++i) {
			if (pf->have_encodetable && (pf->offset[i] ==
			    pf->offset[pf->defaultchar-pf->firstchar])) {
				if (did_defaultchar) {
					fprintf(ofp, "  %ld,\t/* (0x%02x) */\n",
						default_offset,
						i + pf->firstchar);
					continue;
				}
				did_defaultchar = 1;
				default_offset = offset;
			}
			fprintf(ofp, "  %ld,\t/* (0x%02x) */\n", offset,
				i + pf->firstchar);
			offset += pf->height;
		}
		fprintf(ofp, "};\n\n");
	}

	/* output width table for proportional fonts*/
	if (pf->proportional) {
		fprintf(ofp, 	"/* Character width data. */\n"
			"static const unsigned char font_width[] = {\n");

		for (i=0; i<pf->size; ++i)
			fprintf(ofp, "  %d,\t/* (0x%02x) */\n", pf->width[i], i+pf->firstchar);
		fprintf(ofp, "};\n\n");
	}

	/* output font_t struct*/
	if (pf->have_encodetable)
		sprintf(obuf, "font_offset,");
	else
		sprintf(obuf, "0,  /* no encode table*/");

	if (pf->proportional)
		sprintf(buf, "font_width,");
	else
		sprintf(buf, "0,  /* fixed width*/");

	fprintf(ofp, "/* Exported structure definition. */\n"
		"const gpanel_font_t font = {\n"
		"  \"font\",\n"
		"  %d,\n"
		"  %d,\n"
		"  %d,\n"
		"  %d,\n"
		"  %d,\n"
		"  font_bits,\n"
		"  %s\n"
		"  %s\n"
		"  %d,\n"
		"  sizeof(font_bits) / sizeof(font_bits[0]),\n"
		"};\n",
		pf->maxwidth, pf->height,
		pf->ascent,
		pf->firstchar,
		pf->size,
		obuf,
		buf,
		pf->defaultchar);

	return 0;
}

int
main()
{
	font_t *pf = read_font();
	if (! pf)
		exit(1);

	gen_c_source(pf);

	free(pf);
	return 0;
}
