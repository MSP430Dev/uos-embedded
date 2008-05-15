#include <runtime/lib.h>
#include "nanox/include/nano-X.h"

#include "images.h"

#define GET_IMAGE(name, msg) { \
	GR_IMAGE_INFO info; \
	extern GR_IMAGE_ID name##_image_id; \
	extern int name##_w, name##_h; \
	name##_image_id = GrLoadImageFromBuffer ((char*) name##_gif, \
		sizeof(name##_gif), 0); \
        if (! name##_image_id) { \
                debug_printf ("Can't load " msg " image file\n"); \
                abort(); \
        } \
        GrGetImageInfo (name##_image_id, &info); \
        name##_w = info.width; \
        name##_h = info.height; \
	}

int load_images(void)
{
	/* Use embedded images. */
	GET_IMAGE (board, "board");
	GET_IMAGE (w_p, "white pawn");
	GET_IMAGE (w_n, "white knight");
	GET_IMAGE (w_b, "white bishop");
	GET_IMAGE (w_r, "white rook");
	GET_IMAGE (w_k, "white king");
	GET_IMAGE (w_q, "white queen");
	GET_IMAGE (b_p, "black pawn");
	GET_IMAGE (b_n, "black knight");
	GET_IMAGE (b_b, "black bishop");
	GET_IMAGE (b_r, "black rook");
	GET_IMAGE (b_k, "black king");
	GET_IMAGE (b_q, "black queen");

	return(0);
}
