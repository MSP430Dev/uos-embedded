/*
 * NanoBreaker, a Nano-X Breakout clone by Alex Holden.
 *
 * The contents of this file are subject to the Mozilla Public License
 * Version 1.1 (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS"
 * basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
 * License for the specific language governing rights and limitations
 * under the License.
 *
 * The Original Code is NanoBreaker.
 *
 * The Initial Developer of the Original Code is Alex Holden.
 * Portions created by Alex Holden are Copyright (C) 2002
 * Alex Holden <alex@alexholden.net>. All Rights Reserved.
 *
 * Contributor(s):
 *
 * Alternatively, the contents of this file may be used under the terms
 * of the GNU General Public license (the  "[GNU] License"), in which case the
 * provisions of [GNU] License are applicable instead of those
 * above.  If you wish to allow use of your version of this file only
 * under the terms of the [GNU] License and not to allow others to use
 * your version of this file under the MPL, indicate your decision by
 * deleting  the provisions above and replace  them with the notice and
 * other provisions required by the [GNU] License.  If you do not delete
 * the provisions above, a recipient may use your version of this file
 * under either the MPL or the [GNU] License.
 */

/* sprite.c contains sprite creation and destruction functions. NanoBreaker
 * sprites basically consist of a Nano-X pixmap and a Nano-X alpha channel,
 * with their dimensions and the name of the file they were loaded from.
 * A list is kept of all the sprites, and when a sprite is asked to be created
 * from an image which has already been loaded (with the same dimensions) the
 * existing sprite is returned instead and the sprites usage count incremented.
 * When a sprite is asked to be destroyed, the usage count is decremented and
 * the sprite is only actually destroyed when the count reaches 0. */

#include <runtime/lib.h>
#include <posix/stdlib.h>
#include <posix/stdio.h>
#include <posix/string.h>

#include "nanox/include/nano-X.h"
#include "nanox/include/nxcolors.h"
#include "nbreaker.h"

#ifndef HAVE_FILEIO
#include "images.h"
#endif

/* Create a new sprite structure with the specified file name, dimensions,
 * and pixmap/alpha channel IDs, then set the usage count to 1 and link it
 * into the sprite list. Called by make_empty_sprite() and load_sprite(). */
static sprite *make_sprite(nbstate *state, char *fname, int w, int h,
		GR_DRAW_ID p, GR_DRAW_ID a)
{
	sprite *s;

	/* Allocate the sprite structure: */
	if(!(s = malloc(sizeof(sprite)))) {
		oom();
		return 0;
	}

	/* Copy the file name string: */
	if(!fname) s->fname = 0;
	else {
		if(!(s->fname = strdup(fname))) {
			oom();
			free(s);
			return 0;
		}
	}

	/* Set the various parameters: */
	s->w = w;
	s->h = h;
	s->p = p;
	s->a = a;
	s->usage = 1;

	/* Link it into the sprite list: */
	s->next = state->spritelist;
	if(s->next) s->next->prev = s;
	s->prev = 0;
	state->spritelist = s;

	return s; /* Return the address of the new sprite structure. */
}

/* Make an empty sprite structure with the specified file name, width, and
 * height. It is expected that you will probably already have tried calling
 * load_sprite() first and it failed, so you want to create the sprite
 * manually. */
sprite *make_empty_sprite(nbstate *state, char *fname, int width, int height)
{
	sprite *s;
	GR_DRAW_ID a;
	GR_DRAW_ID p;

	/* Allocate the alpha channel and pixmap with the specified
	 * dimensions (which must be valid): */
	if(!(a = GrNewPixmap(width, height, 0)))
		return 0;
	if(!(p = GrNewPixmap(width, height, 0))) {
		GrDestroyWindow(a);
		return 0;
	}

	/* Make the sprite itself and return it to the caller: */
	if(!(s = make_sprite(state, fname, width, height, p, a))) {
		GrDestroyWindow(a);
		GrDestroyWindow(p);
		return 0;
	} else return s;
}

/* Create a new sprite from the specified image file. Returns the address of
 * the new sprite on success or 0 on failure. If width is -1, the real
 * dimensions of the image file will be used, otherwise the image will be
 * scaled up or down to the specified dimensions. */
sprite *load_sprite(nbstate *state, char *fname, int width, int height)
{
	sprite *s;
	GR_DRAW_ID a;
	GR_DRAW_ID p;
	GR_IMAGE_ID img;
	GR_IMAGE_INFO ii;

	/* Make sure the file name has been specified: */
	if (! fname) return 0;

	/* Try to find a sprite in the list with the specified filename and
	 * dimensions (any dimensions are OK if width is -1). If one is found,
	 * increment its usage count and return its address. */
	for(s = state->spritelist; s; s = s->next) {
		if ((width == -1 || (width == s->w && height == s->h)) &&
		    s->fname && !strcmp(fname, s->fname)) {
			s->usage++;
			return s;
		}
	}
#ifdef HAVE_FILEIO
	{
	/* Make the full path to the filename because the Nano-X server
	 * probably isn't running from the game data directory: */
	char buf[256];
	if (snprintf(buf, 256, "%s/%s", state->gamedir, fname) >= 256){
		debug_printf ("Warning: image path \"%s/%s\" is too long\n",
				state->gamedir, fname);
		return 0;
	}

	/* Try to load the image file, and return 0 on failure: */
	img = GrLoadImageFromFile (buf, 0);
	}
#else
	if      (strcmp (fname,    "nbb1.gif") == 0) img = GrLoadImageFromBuffer ((char*)   nbb1_data, sizeof(   nbb1_data), 0);
	else if (strcmp (fname,    "nbb2.gif") == 0) img = GrLoadImageFromBuffer ((char*)   nbb2_data, sizeof(   nbb2_data), 0);
	else if (strcmp (fname,    "nbb3.gif") == 0) img = GrLoadImageFromBuffer ((char*)   nbb3_data, sizeof(   nbb3_data), 0);
	else if (strcmp (fname,    "nbb4.gif") == 0) img = GrLoadImageFromBuffer ((char*)   nbb4_data, sizeof(   nbb4_data), 0);
	else if (strcmp (fname,    "nbb5.gif") == 0) img = GrLoadImageFromBuffer ((char*)   nbb5_data, sizeof(   nbb5_data), 0);
	else if (strcmp (fname,    "nbb6.gif") == 0) img = GrLoadImageFromBuffer ((char*)   nbb6_data, sizeof(   nbb6_data), 0);
	else if (strcmp (fname,    "nbb7.gif") == 0) img = GrLoadImageFromBuffer ((char*)   nbb7_data, sizeof(   nbb7_data), 0);
	else if (strcmp (fname,    "nbb8.gif") == 0) img = GrLoadImageFromBuffer ((char*)   nbb8_data, sizeof(   nbb8_data), 0);
	else if (strcmp (fname,    "nbb9.gif") == 0) img = GrLoadImageFromBuffer ((char*)   nbb9_data, sizeof(   nbb9_data), 0);
	else if (strcmp (fname, "nbball1.gif") == 0) img = GrLoadImageFromBuffer ((char*)nbball1_data, sizeof(nbball1_data), 0);
	else if (strcmp (fname,  "nbbat1.gif") == 0) img = GrLoadImageFromBuffer ((char*) nbbat1_data, sizeof( nbbat1_data), 0);
	else if (strcmp (fname,  "nbbat2.gif") == 0) img = GrLoadImageFromBuffer ((char*) nbbat2_data, sizeof( nbbat2_data), 0);
	else if (strcmp (fname,  "nbbat3.gif") == 0) img = GrLoadImageFromBuffer ((char*) nbbat3_data, sizeof( nbbat3_data), 0);
	else if (strcmp (fname,  "nbbg10.gif") == 0) img = GrLoadImageFromBuffer ((char*) nbbg10_data, sizeof( nbbg10_data), 0);
	else if (strcmp (fname,   "nbbg1.gif") == 0) img = GrLoadImageFromBuffer ((char*)  nbbg1_data, sizeof(  nbbg1_data), 0);
	else if (strcmp (fname,   "nbbg2.gif") == 0) img = GrLoadImageFromBuffer ((char*)  nbbg2_data, sizeof(  nbbg2_data), 0);
	else if (strcmp (fname,   "nbbg3.gif") == 0) img = GrLoadImageFromBuffer ((char*)  nbbg3_data, sizeof(  nbbg3_data), 0);
	else if (strcmp (fname,   "nbbg4.gif") == 0) img = GrLoadImageFromBuffer ((char*)  nbbg4_data, sizeof(  nbbg4_data), 0);
	else if (strcmp (fname,   "nbbg5.gif") == 0) img = GrLoadImageFromBuffer ((char*)  nbbg5_data, sizeof(  nbbg5_data), 0);
	else if (strcmp (fname,   "nbbg6.gif") == 0) img = GrLoadImageFromBuffer ((char*)  nbbg6_data, sizeof(  nbbg6_data), 0);
	else if (strcmp (fname,   "nbbg7.gif") == 0) img = GrLoadImageFromBuffer ((char*)  nbbg7_data, sizeof(  nbbg7_data), 0);
	else if (strcmp (fname,   "nbbg8.gif") == 0) img = GrLoadImageFromBuffer ((char*)  nbbg8_data, sizeof(  nbbg8_data), 0);
	else if (strcmp (fname,   "nbbg9.gif") == 0) img = GrLoadImageFromBuffer ((char*)  nbbg9_data, sizeof(  nbbg9_data), 0);
	else if (strcmp (fname,    "nbpf.gif") == 0) img = GrLoadImageFromBuffer ((char*)   nbpf_data, sizeof(   nbpf_data), 0);
	else if (strcmp (fname,    "nbpn.gif") == 0) img = GrLoadImageFromBuffer ((char*)   nbpn_data, sizeof(   nbpn_data), 0);
	else if (strcmp (fname,    "nbpp.gif") == 0) img = GrLoadImageFromBuffer ((char*)   nbpp_data, sizeof(   nbpp_data), 0);
	else if (strcmp (fname,    "nbps.gif") == 0) img = GrLoadImageFromBuffer ((char*)   nbps_data, sizeof(   nbps_data), 0);
	else if (strcmp (fname,    "nbpt.gif") == 0) img = GrLoadImageFromBuffer ((char*)   nbpt_data, sizeof(   nbpt_data), 0);
	else if (strcmp (fname,    "nbpw.gif") == 0) img = GrLoadImageFromBuffer ((char*)   nbpw_data, sizeof(   nbpw_data), 0);
	else if (strcmp (fname,   "nbsp1.gif") == 0) img = GrLoadImageFromBuffer ((char*)  nbsp1_data, sizeof(  nbsp1_data), 0);
	else if (strcmp (fname,   "nbsp2.gif") == 0) img = GrLoadImageFromBuffer ((char*)  nbsp2_data, sizeof(  nbsp2_data), 0);
	else if (strcmp (fname,   "nbsp3.gif") == 0) img = GrLoadImageFromBuffer ((char*)  nbsp3_data, sizeof(  nbsp3_data), 0);
	else {
		debug_printf ("No such image: \"%s\"\n", fname);
		abort ();
		return 0;
	}
#endif
	if (! img) {
		debug_printf ("Warning: failed to load image \"%s\"- make "
				"sure it is where the server can find it and "
				"that support for loading the relevant image "
				"type has been built into the server\n", fname);
		return 0;
	}

	/* If a size wasn't specified, get the real image size from the server
	 * instead: */
	if(width == -1 || height == -1) {
		GrGetImageInfo(img, &ii);
		width = ii.width;
		height = ii.height;
	}

	/* Make the alpha channel and pixmap to store the image in: */
	if(!(a = GrNewPixmap(width, height, 0))) {
		GrFreeImage(img);
		return 0;
	}
	if(!(p = GrNewPixmap(width, height, 0))) {
		GrFreeImage(img);
		GrDestroyWindow(a);
		return 0;
	}

	/* Draw the image into the specified pixmap and alpha channel, scaling
	 * it up or down if necessary: */
	GrDrawImageToFit(p, state->gc, 0, 0, width, height, img);
	GrFreeImage(img); /* Destroy the server image object. */

	/* Make a new sprite and link it into the list, then return its
	 * address to the caller: */
	s = make_sprite(state, fname, width, height, p, a);
	if (! s) {
		GrDestroyWindow(a);
		GrDestroyWindow(p);
		return 0;
	}

	return s;
}

/* Destroy the specified sprite. Should be called whenever you don't need a
 * particular sprite any more, even though it doesn't actually destroy the
 * sprite until all users of the sprite have called destroy_sprite(). */
void destroy_sprite(nbstate *state, sprite *s)
{
	if(!s) return; /* Sanity check. */

	if(!--s->usage) { /* Decrement the usage count. */
		/* The usage count is zero, so destroy the sprite: */
		myfree(s->fname); /* Free the file name. */
		GrDestroyWindow(s->p); /* Destroy the pixmap. */
		GrDestroyWindow(s->a); /* Destroy the alpha channel. */
		/* Unlink it from the sprite list: */
		if(s == state->spritelist) state->spritelist = s->next;
		else s->prev->next = s->next;
		if(s->next) s->next->prev = s->prev;
		free(s); /* Free the structure itself. */
	}
}

/* Destroy every sprite in the sprite list in one go. More efficient than
 * calling destroy_sprite() for every use of every sprite. */
void destroy_all_sprites(nbstate *state)
{
	sprite *s, *snext = state->spritelist;

	while((s = snext)) { /* Iterate through the list. */
		myfree(s->fname); /* Free the file name. */
		GrDestroyWindow(s->p); /* Destroy the pixmap. */
		GrDestroyWindow(s->a); /* Destroy the alpha channel. */
		snext = s->next;
		free(s); /* Free the structure itself. */
	}
	state->spritelist = 0; /* Set the list head empty. */
}
