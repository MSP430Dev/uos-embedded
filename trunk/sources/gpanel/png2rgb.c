#include <stdlib.h>
#include <png.h>

int width, height;
png_bytep *row_pointers;

void read_png_file (char *file_name)
{
	png_structp png_ptr;
	png_infop info_ptr;
	unsigned char header [8];	// 8 is the maximum size that can be checked
	int y, number_of_passes;

	FILE *fp = fopen (file_name, "rb");
	if (! fp) {
		perror (file_name);
		exit (-1);
	}
	if (fread (header, 1, sizeof (header), fp) != sizeof (header)) {
		fprintf (stderr, "%s: too short file\n", file_name);
		exit (-1);
	}
	if (png_sig_cmp (header, 0, 8)) {
		fprintf (stderr, "%s: not a PNG file\n", file_name);
		exit (-1);
	}

	png_ptr = png_create_read_struct (PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (png_ptr)
		info_ptr = png_create_info_struct (png_ptr);
	if (! png_ptr || ! info_ptr) {
		fprintf (stderr, "%s: cannot create PNG structs\n", file_name);
		exit (-1);
	}
	if (setjmp (png_jmpbuf (png_ptr))) {
		fprintf (stderr, "%s: init PNG failed\n", file_name);
		exit (-1);
	}
	png_init_io (png_ptr, fp);
	png_set_sig_bytes (png_ptr, 8);

	png_read_info (png_ptr, info_ptr);
	width = info_ptr->width;
	height = info_ptr->height;
	number_of_passes = png_set_interlace_handling (png_ptr);
	png_read_update_info (png_ptr, info_ptr);

	/*printf ("%s: width %d, height %d\n", file_name, width, height);*/
	/*printf ("Color type %d, bit depth %d\n", info_ptr->color_type, info_ptr->bit_depth);*/
	/*printf ("Row length is %lu bytes\n", info_ptr->rowbytes);*/
	/*printf ("Number of passes = %d\n", number_of_passes);*/
	if (info_ptr->color_type != PNG_COLOR_TYPE_RGB) {
		fprintf (stderr, "%s: incorrect color type %d (expected %d)\n",
			file_name, info_ptr->color_type, PNG_COLOR_TYPE_RGB);
		exit (-1);
	}
	if (info_ptr->bit_depth != 8) {
		fprintf (stderr, "%s: incorrect bit depth %d (expected 8)\n",
			file_name, info_ptr->bit_depth);
		exit (-1);
	}

	row_pointers = (png_bytep*) malloc (sizeof(png_bytep) * height);
	for (y=0; y<height; y++)
		row_pointers[y] = (png_byte*) malloc (info_ptr->rowbytes);
	if (setjmp (png_jmpbuf (png_ptr))) {
		fprintf (stderr, "%s: reading PNG failed\n", file_name);
		exit (-1);
	}
	png_read_image (png_ptr, row_pointers);

        fclose (fp);
	png_destroy_read_struct (&png_ptr, &info_ptr, NULL);
}

unsigned scale (int level)
{
	unsigned x;
#if 0
	x = (level + 8) / 16;
	if (x > 15)
		return 15;
#endif
	x = level >> 4;
	return x;
}

void process_file (char *file_name)
{
	int name_len, x, y;
	char *dot;

	dot = strchr (file_name, '.');
	if (dot)
		name_len = dot - file_name;
	else
		name_len = strlen (file_name);
	printf ("/* This image was generated from %s by png2rgb utility. */\n",
		file_name);
	printf ("const unsigned short image_%.*s [] = {\n", name_len, file_name);

	for (y=0; y<height; y++) {
		printf ("/*%d*/", y);
		for (x=0; x<width; x++) {
			png_byte *ptr = row_pointers[y] + x*3;
			unsigned r = scale (ptr[0]);
			unsigned g = scale (ptr[1]);
			unsigned b = scale (ptr[2]);

			if (x % 10 == 0)
				printf ("\t");
			printf (" 0x%03x,", (r << 8) | (g << 4) | b);
			if (x % 10 == 9)
				printf ("\n");
		}
		if (x % 10 != 0)
			printf ("\n");
	}
	printf ("};\n");
}

int main (int argc, char **argv)
{
	if (argc != 2) {
		fprintf (stderr, "Usage:\n");
		fprintf (stderr, "       png2rgb input.png\n");
		exit (-1);
	}
	read_png_file (argv[1]);
	process_file (argv[1]);
        return 0;
}
