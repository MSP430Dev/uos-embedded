/*
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
 * The Original Code is NanoTetris.
 *
 * The Initial Developer of the Original Code is Alex Holden.
 * Portions created by Alex Holden are Copyright (C) 2000, 2002
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

/*
 * A Nano-X Tetris clone by Alex Holden.
 *
 * The objective is to keep placing new pieces for as long as possible. When a
 * horizontal line is filled with blocks, it will vanish, and everything above
 * it will drop down a line. It quickly gets difficult because the speed
 * increases with the score. Unlike with some Tetris clones, no bonus points
 * are awarded for matching colours, completing more than one line at a time,
 * or for using the "drop shape to bottom" function.
 *
 * The box in the top left of the game window is the score box. The top score
 * is the highest score you have achieved since last resetting the high score
 * counter. The counter is stored when the game exits in the file specified by
 * the HISCORE_FILE parameter ("/usr/games/nanotetris.hiscore" by default).
 * Note that no attempt is made to encrypt the file, so anybody with write
 * access to the file can alter the contents of it using a text editor.
 *
 * The box below the score box is the next shape box. This contains a "preview"
 * of the next shape to appear, so that you can plan ahead as you are building
 * up the blocks.
 *
 * The game functions can be controlled using either the mouse (or a touch pad,
 * touch screen, trackball, etc.) and the buttons below the next shape box, or
 * with the following keyboard keys:
 *
 *   Q = quit game
 *   N = new game
 *   P = pause game
 *   C = continue game
 *   D = rotate shape anticlockwise
 *   F = rotate shape clockwise
 *   J = move shape left
 *   K = move shape right
 *   Space Bar = drop shape to bottom.
 *
 * The reason for the unconventional use of D, F, J, and K keys is that they
 * are all in the "home" position of a QWERTY keyboard, which makes them very
 * easy to press if you are used to touch typing.
 *
 * I'll leave it to you to figure out which mouse operated movement button does
 * what (it's pretty obvious).
 */
#include <runtime/lib.h>
#include <kernel/uos.h>
#include <timer/timer.h>
#include <stream/stream.h>
#include <random/rand15.h>

#include "nanox/include/nano-X.h"
#include "nanox/include/nxcolors.h"

#include "ntetris.h"

extern timer_t *uos_timer;

static void draw_shape(nstate *state, GR_COORD x, GR_COORD y, int erase)
{
	int r, c, yy, xx;
	GR_COLOR col;
	char ch = 0;

	if(erase) col = 0;
	else col = state->current_shape.colour;

	for(r = 0; ch < 3; r++) {
		ch = 0;
		for(c = 0; ch < 2; c++) {
			ch = shapes[state->current_shape.type]
				[state->current_shape.orientation][r][c];
			if(ch == 1) {
				yy = y + r;
				xx = x + c;
				state->blocks[0][yy][xx] = col;
			}
		}
	}
}

static int will_collide(nstate *state, int x, int y, int orientation)
{
	int r, c, xx, yy;
	char ch = 0;

	draw_shape(state, state->current_shape.x, state->current_shape.y, 1);
	for(r = 0; ch < 3; r++) {
		ch = 0;
		for(c = 0; ch < 2; c++) {
			ch = shapes[state->current_shape.type]
				[orientation][r][c];
			if(ch == 1) {
				yy = y + r;
				xx = x + c;
				if((yy == WELL_HEIGHT) || (xx == WELL_WIDTH) ||
						(state->blocks[0][yy][xx])) {
					draw_shape(state,
						state->current_shape.x,
						state->current_shape.y, 0);
					return 1;
				}
			}
		}
	}
	draw_shape(state, state->current_shape.x, state->current_shape.y, 0);

	return 0;
}

static void draw_well(nstate *state, int forcedraw)
{
	int x, y;

	for(y = WELL_NOTVISIBLE; y < WELL_HEIGHT; y++) {
		for(x = 0; x < WELL_WIDTH; x++) {
			if(forcedraw || (state->blocks[0][y][x] !=
						state->blocks[1][y][x])) {
				state->blocks[1][y][x] = state->blocks[0][y][x];
				GrSetGCForeground(state->wellgc,
							state->blocks[0][y][x]);
				GrFillRect(state->well_window, state->wellgc,
					(BLOCK_SIZE * x),
					(BLOCK_SIZE * (y - WELL_NOTVISIBLE)),
						BLOCK_SIZE, BLOCK_SIZE);
			}
		}
	}

	GrFlush();
}

static void draw_score(nstate *state)
{
	unsigned char buf[32];

	GrFillRect(state->score_window, state->scoregcb, 0, 0,
			SCORE_WINDOW_WIDTH, SCORE_WINDOW_HEIGHT);

	snprintf (buf, sizeof (buf), "%d", state->score);
	GrText(state->score_window, state->scoregcf, TEXT_X_POSITION,
					TEXT2_Y_POSITION, buf, strlen(buf), 0);
	snprintf (buf, sizeof (buf), "%d", state->hiscore);
	GrText(state->score_window, state->scoregcf, TEXT_X_POSITION,
					TEXT_Y_POSITION, buf, strlen(buf), 0);
}

static void draw_next_shape(nstate *state)
{
	int r, c, startx, starty, x, y;
	char ch = 0;

	GrFillRect(state->next_shape_window, state->nextshapegcb, 0, 0,
			NEXT_SHAPE_WINDOW_WIDTH, NEXT_SHAPE_WINDOW_HEIGHT);

	GrSetGCForeground(state->nextshapegcf, state->next_shape.colour);

	startx = (BLOCK_SIZE * ((NEXT_SHAPE_WINDOW_SIZE / 2) -
			(shape_sizes[state->next_shape.type]
			[state->next_shape.orientation][0] / 2)));
	starty = (BLOCK_SIZE * ((NEXT_SHAPE_WINDOW_SIZE / 2) -
			(shape_sizes[state->next_shape.type]
			[state->next_shape.orientation][1] / 2)));

	for(r = 0; ch < 3; r++) {
		ch = 0;
		for(c = 0; ch < 2; c++) {
			ch = shapes[state->next_shape.type]
				[state->next_shape.orientation][r][c];
			if(ch == 1) {
				x = startx + (c * BLOCK_SIZE);
				y = starty + (r * BLOCK_SIZE);
				GrFillRect(state->next_shape_window,
					state->nextshapegcf, x, y,
					BLOCK_SIZE, BLOCK_SIZE);
			}
		}
	}
}

static void draw_new_game_button(nstate *state)
{
	GrFillRect(state->new_game_button, state->buttongcb, 0, 0,
			NEW_GAME_BUTTON_WIDTH, NEW_GAME_BUTTON_HEIGHT);
	GrText(state->new_game_button, state->buttongcf, TEXT_X_POSITION,
					TEXT_Y_POSITION, "New Game", 8, 0);
}

static void draw_anticlockwise_button(nstate *state)
{
	if(!state->running_buttons_mapped) return;
	GrFillRect(state->anticlockwise_button, state->buttongcb, 0, 0,
		ANTICLOCKWISE_BUTTON_WIDTH, ANTICLOCKWISE_BUTTON_HEIGHT);
	GrText(state->anticlockwise_button, state->buttongcf, TEXT_X_POSITION,
					TEXT_Y_POSITION, "   /", 4, 0);
}

static void draw_clockwise_button(nstate *state)
{
	if(!state->running_buttons_mapped) return;
	GrFillRect(state->clockwise_button, state->buttongcb, 0, 0,
			CLOCKWISE_BUTTON_WIDTH, CLOCKWISE_BUTTON_HEIGHT);
	GrText(state->clockwise_button, state->buttongcf, TEXT_X_POSITION,
					TEXT_Y_POSITION, "   \\", 4, 0);
}

static void draw_left_button(nstate *state)
{
	if(!state->running_buttons_mapped) return;
	GrFillRect(state->left_button, state->buttongcb, 0, 0,
			LEFT_BUTTON_WIDTH, LEFT_BUTTON_HEIGHT);
	GrText(state->left_button, state->buttongcf, TEXT_X_POSITION,
					TEXT_Y_POSITION, "  <", 3, 0);
}

static void draw_right_button(nstate *state)
{
	if(!state->running_buttons_mapped) return;
	GrFillRect(state->right_button, state->buttongcb, 0, 0,
			RIGHT_BUTTON_WIDTH, RIGHT_BUTTON_HEIGHT);
	GrText(state->right_button, state->buttongcf, TEXT_X_POSITION,
					TEXT_Y_POSITION, "   >", 4, 0);
}

static void draw_drop_button(nstate *state)
{
	if(!state->running_buttons_mapped) return;
	GrFillRect(state->drop_button, state->buttongcb, 0, 0,
			DROP_BUTTON_WIDTH, DROP_BUTTON_HEIGHT);
	GrText(state->drop_button, state->buttongcf, TEXT_X_POSITION,
					TEXT_Y_POSITION, "    Drop", 8, 0);
}

static void draw_pause_continue_button(nstate *state)
{
	if((state->running_buttons_mapped) && (state->state == STATE_STOPPED)) {
		GrUnmapWindow(state->pause_continue_button);
		GrUnmapWindow(state->anticlockwise_button);
		GrUnmapWindow(state->clockwise_button);
		GrUnmapWindow(state->left_button);
		GrUnmapWindow(state->right_button);
		GrUnmapWindow(state->drop_button);
		state->running_buttons_mapped = 0;
		return;
	}
	if((!state->running_buttons_mapped) && (state->state == STATE_RUNNING)){
		GrMapWindow(state->pause_continue_button);
		GrMapWindow(state->anticlockwise_button);
		GrMapWindow(state->clockwise_button);
		GrMapWindow(state->left_button);
		GrMapWindow(state->right_button);
		GrMapWindow(state->drop_button);
		state->running_buttons_mapped = 1;
		return;
	}
	if(!state->running_buttons_mapped) return;
	GrFillRect(state->pause_continue_button, state->buttongcb, 0, 0,
		PAUSE_CONTINUE_BUTTON_WIDTH, PAUSE_CONTINUE_BUTTON_HEIGHT);
	if(state->state == STATE_PAUSED) {
		GrText(state->pause_continue_button, state->buttongcf,
			TEXT_X_POSITION, TEXT_Y_POSITION, " Continue", 9, 0);
	} else {
		GrText(state->pause_continue_button, state->buttongcf,
			TEXT_X_POSITION, TEXT_Y_POSITION, "   Pause", 8, 0);
	}
}

static int block_is_all_in_well(nstate *state)
{
	if(state->current_shape.y >= WELL_NOTVISIBLE)
		return 1;

	return 0;
}

static void delete_line(nstate *state, int line)
{
	int x, y;

	if(line < WELL_NOTVISIBLE) return;

	for(y = line - 1; y; y--)
		for(x = WELL_WIDTH; x; x--)
			state->blocks[0][y + 1][x] = state->blocks[0][y][x];

	draw_well(state, 0);
}

static void choose_new_shape(nstate *state)
{
	state->current_shape.type = state->next_shape.type;
	state->current_shape.orientation = state->next_shape.orientation;
	state->current_shape.colour = state->next_shape.colour;
	state->current_shape.x = (WELL_WIDTH / 2) - 2;
	state->current_shape.y = WELL_NOTVISIBLE -
			shape_sizes[state->next_shape.type]
				[state->next_shape.orientation][1] - 1;
	state->next_shape.type = rand15() % MAXSHAPES;
	state->next_shape.orientation = rand15() % MAXORIENTATIONS;
	state->next_shape.colour = block_colours [state->next_shape.type];
}

static void block_reached_bottom(nstate *state)
{
	int x, y, nr;

	if(!block_is_all_in_well(state)) {
		state->state = STATE_STOPPED;
		return;
	}

	for(y = WELL_HEIGHT - 1; y; y--) {
		nr = 0;
		for(x = 0; x < WELL_WIDTH; x++) {
			if(!state->blocks[0][y][x]) {
				nr = 1;
				break;
			}
		}
		if(nr) continue;
		timer_delay (uos_timer, DELETE_LINE_DELAY);
		delete_line(state, y);
		state->score += SCORE_INCREMENT;
		if((LEVELS > (state->level + 1)) && (((state->level + 1) *
					LEVEL_DIVISOR) <= state->score))
			state->level++;
		draw_score(state);
		y++;
	}

	choose_new_shape(state);
	draw_next_shape(state);
}

static void move_block(nstate *state, int direction)
{
	if(direction == 0) {
 		if(!state->current_shape.x) return;
		else {
			if(!will_collide(state, (state->current_shape.x - 1),
						state->current_shape.y,
					state->current_shape.orientation)) {
				draw_shape(state, state->current_shape.x,
						state->current_shape.y, 1);
				state->current_shape.x--;
				draw_shape(state, state->current_shape.x,
						state->current_shape.y, 0);
				draw_well(state, 0);
			}
		}
	} else {
		if(!will_collide(state, (state->current_shape.x + 1),
						state->current_shape.y,
					state->current_shape.orientation)) {
			draw_shape(state, state->current_shape.x,
					state->current_shape.y, 1);
			state->current_shape.x++;
			draw_shape(state, state->current_shape.x,
					state->current_shape.y, 0);
			draw_well(state, 0);
		}
	}
}

static void rotate_block(nstate *state, int direction)
{
	int neworientation = 0;

	if(direction == 0) {
		if(!state->current_shape.orientation)
			neworientation = MAXORIENTATIONS - 1;
		else neworientation = state->current_shape.orientation - 1;
	} else {
		neworientation = state->current_shape.orientation + 1;
		if(neworientation == MAXORIENTATIONS) neworientation = 0;
	}

	if(!will_collide(state, state->current_shape.x, state->current_shape.y,
							neworientation)) {
		draw_shape(state, state->current_shape.x,
				state->current_shape.y, 1);
		state->current_shape.orientation = neworientation;
		draw_shape(state, state->current_shape.x,
				state->current_shape.y, 0);
		draw_well(state, 0);
	}
}

static int drop_block_1(nstate *state)
{
	if(will_collide(state, state->current_shape.x,
				(state->current_shape.y + 1),
				state->current_shape.orientation)) {
		block_reached_bottom(state);
		return 1;
	}

	draw_shape(state, state->current_shape.x, state->current_shape.y, 1);
	state->current_shape.y++;
	draw_shape(state, state->current_shape.x, state->current_shape.y, 0);

	draw_well(state, 0);

	return 0;
}

static void drop_block(nstate *state)
{
	while(!drop_block_1(state))
		timer_delay (uos_timer, DROP_BLOCK_DELAY);
}

static void handle_exposure_event(nstate *state)
{
	GR_EVENT_EXPOSURE *event = &state->event.exposure;

	if(event->wid == state->score_window) {
		draw_score(state);
		return;
	}
	if(event->wid == state->next_shape_window) {
		draw_next_shape(state);
		return;
	}
	if(event->wid == state->new_game_button) {
		draw_new_game_button(state);
		return;
	}
	if(event->wid == state->pause_continue_button) {
		draw_pause_continue_button(state);
		return;
	}
	if(event->wid == state->anticlockwise_button) {
		draw_anticlockwise_button(state);
		return;
	}
	if(event->wid == state->clockwise_button) {
		draw_clockwise_button(state);
		return;
	}
	if(event->wid == state->left_button) {
		draw_left_button(state);
		return;
	}
	if(event->wid == state->right_button) {
		draw_right_button(state);
		return;
	}
	if(event->wid == state->drop_button) {
		draw_drop_button(state);
		return;
	}
	if(event->wid == state->well_window) {
		draw_well(state, 1);
		return;
	}
}

static void handle_mouse_event(nstate *state)
{
	GR_EVENT_MOUSE *event = &state->event.mouse;

	if(event->wid == state->new_game_button) {
		state->state = STATE_NEWGAME;
		return;
	}
	if(event->wid == state->pause_continue_button) {
		if(state->state == STATE_PAUSED) state->state = STATE_RUNNING;
		else state->state = STATE_PAUSED;
		return;
	}
	if(event->wid == state->anticlockwise_button) {
		if(state->state == STATE_PAUSED) state->state = STATE_RUNNING;
		rotate_block(state, 0);
		return;
	}
	if(event->wid == state->clockwise_button) {
		if(state->state == STATE_PAUSED) state->state = STATE_RUNNING;
		rotate_block(state, 1);
		return;
	}
	if(event->wid == state->left_button) {
		if(state->state == STATE_PAUSED) state->state = STATE_RUNNING;
		move_block(state, 0);
		return;
	}
	if(event->wid == state->right_button) {
		if(state->state == STATE_PAUSED) state->state = STATE_RUNNING;
		move_block(state, 1);
		return;
	}
	if(event->wid == state->drop_button) {
		if(state->state == STATE_PAUSED) state->state = STATE_RUNNING;
		drop_block(state);
		return;
	}
}

static void clear_well(nstate *state)
{
	int x, y;

	for(y = 0; y < WELL_HEIGHT; y++)
		for(x = 0; x < WELL_WIDTH; x++) {
			state->blocks[0][y][x] = 0;
			state->blocks[1][y][x] = 0;
		}
}

static void new_game(nstate *state)
{
	clear_well(state);
	if(state->score > state->hiscore) state->hiscore = state->score;
	state->score = 0;
	state->level = 0;
	draw_score(state);
	choose_new_shape(state);
	draw_next_shape(state);
	draw_well(state, 1);
	if(state->state == STATE_NEWGAME) state->state = STATE_RUNNING;
}

static void handle_keyboard_event(nstate *state)
{
	GR_EVENT_KEYSTROKE *event = &state->event.keystroke;

	switch(event->ch) {
		case 'q':
		case 'Q':
		case MWKEY_CANCEL:
			/* No exit from game. */
			new_game (state);
			state->state = STATE_PAUSED;
			/*state->state = STATE_EXIT;*/
			return;
		case 'n':
		case 'N':
		case MWKEY_APP1:
			state->state = STATE_NEWGAME;
			return;
	}

	if(state->state == STATE_STOPPED) return;

	state->state = STATE_RUNNING;

	switch(event->ch) {
		case 'p':
		case 'P':
			state->state = STATE_PAUSED;
			break;
		case 'j':
		case 'J':
        	case MWKEY_LEFT:
			move_block(state, 0);
			break;
		case 'k':
		case 'K':
        	case MWKEY_RIGHT:
			move_block(state, 1);
			break;
		case 'd':
		case 'D':
        	case MWKEY_UP:
			rotate_block(state, 0);
			break;
		case 'f':
		case 'F':
        	case MWKEY_DOWN:
			rotate_block(state, 1);
			break;
		case ' ':
        	case MWKEY_MENU:
			drop_block(state);
			break;
	}
}

static void handle_event(nstate *state)
{
	switch(state->event.type) {
		case GR_EVENT_TYPE_EXPOSURE:
			handle_exposure_event(state);
			break;
		case GR_EVENT_TYPE_BUTTON_DOWN:
			handle_mouse_event(state);
			break;
		case GR_EVENT_TYPE_KEY_DOWN:
			handle_keyboard_event(state);
			break;
		case GR_EVENT_TYPE_CLOSE_REQ:
			GrClose();
			task_exit (0);
			break;
		case GR_EVENT_TYPE_TIMER:
			break;
		default:
			debug_printf ("Unhandled event type %d\n",
							state->event.type);
			break;
	}
}

static void init_game(nstate *state)
{
	GR_SCREEN_INFO si;

	if (GrOpen() < 0) {
		debug_puts ("Cannot open graphics\n");
		uos_halt (0);
	}
	GrGetScreenInfo (&si);

	state->main_window = GrNewWindowEx (GR_WM_PROPS_APPFRAME |
		GR_WM_PROPS_CAPTION | GR_WM_PROPS_CLOSEBOX,
		(unsigned char*) "Nano-Tetris",
		GR_ROOT_WINDOW_ID, (si.cols - MAIN_WINDOW_WIDTH) / 2,
		(si.rows - MAIN_WINDOW_HEIGHT) / 2,
		MAIN_WINDOW_WIDTH, MAIN_WINDOW_HEIGHT,
		MAIN_WINDOW_BACKGROUND_COLOUR);

	GrSelectEvents (state->main_window, GR_EVENT_MASK_EXPOSURE |
					GR_EVENT_MASK_CLOSE_REQ |
					GR_EVENT_MASK_KEY_DOWN |
					GR_EVENT_MASK_TIMER);
	GrCreateTimer (state->main_window, 50);

	state->score_window = GrNewWindow(state->main_window,
					SCORE_WINDOW_X_POSITION,
					SCORE_WINDOW_Y_POSITION,
					SCORE_WINDOW_WIDTH,
					SCORE_WINDOW_HEIGHT, 0,
					SCORE_WINDOW_BACKGROUND_COLOUR, 0);
	GrSelectEvents(state->score_window, GR_EVENT_MASK_EXPOSURE);
	GrMapWindow(state->score_window);
	state->scoregcf = GrNewGC();
	GrSetGCForeground(state->scoregcf, SCORE_WINDOW_FOREGROUND_COLOUR);
	GrSetGCBackground(state->scoregcf, SCORE_WINDOW_BACKGROUND_COLOUR);
	state->scoregcb = GrNewGC();
	GrSetGCForeground(state->scoregcb, SCORE_WINDOW_BACKGROUND_COLOUR);

	state->next_shape_window = GrNewWindow(state->main_window,
					NEXT_SHAPE_WINDOW_X_POSITION,
					NEXT_SHAPE_WINDOW_Y_POSITION,
					NEXT_SHAPE_WINDOW_WIDTH,
					NEXT_SHAPE_WINDOW_HEIGHT, 0,
					NEXT_SHAPE_WINDOW_BACKGROUND_COLOUR, 0);
	GrSelectEvents(state->next_shape_window, GR_EVENT_MASK_EXPOSURE);
	GrMapWindow(state->next_shape_window);
	state->nextshapegcf = GrNewGC();
	state->nextshapegcb = GrNewGC();
	GrSetGCForeground(state->nextshapegcb,
				NEXT_SHAPE_WINDOW_BACKGROUND_COLOUR);

	state->new_game_button = GrNewWindow(state->main_window,
					NEW_GAME_BUTTON_X_POSITION,
					NEW_GAME_BUTTON_Y_POSITION,
					NEW_GAME_BUTTON_WIDTH,
					NEW_GAME_BUTTON_HEIGHT, 0,
					BUTTON_BACKGROUND_COLOUR, 0);
	GrSelectEvents(state->new_game_button, GR_EVENT_MASK_EXPOSURE |
					GR_EVENT_MASK_BUTTON_DOWN);
	GrMapWindow(state->new_game_button);
	state->buttongcf = GrNewGC();
	GrSetGCForeground(state->buttongcf, BUTTON_FOREGROUND_COLOUR);
	GrSetGCBackground(state->buttongcf, BUTTON_BACKGROUND_COLOUR);
	state->buttongcb = GrNewGC();
	GrSetGCForeground(state->buttongcb, BUTTON_BACKGROUND_COLOUR);

	state->pause_continue_button = GrNewWindow(state->main_window,
					PAUSE_CONTINUE_BUTTON_X_POSITION,
					PAUSE_CONTINUE_BUTTON_Y_POSITION,
					PAUSE_CONTINUE_BUTTON_WIDTH,
					PAUSE_CONTINUE_BUTTON_HEIGHT, 0,
					BUTTON_BACKGROUND_COLOUR, 0);
	GrSelectEvents(state->pause_continue_button, GR_EVENT_MASK_EXPOSURE |
					GR_EVENT_MASK_BUTTON_DOWN);

	state->anticlockwise_button = GrNewWindow(state->main_window,
					ANTICLOCKWISE_BUTTON_X_POSITION,
					ANTICLOCKWISE_BUTTON_Y_POSITION,
					ANTICLOCKWISE_BUTTON_WIDTH,
					ANTICLOCKWISE_BUTTON_HEIGHT, 0,
					BUTTON_BACKGROUND_COLOUR,
					0);
	GrSelectEvents(state->anticlockwise_button, GR_EVENT_MASK_EXPOSURE |
					GR_EVENT_MASK_BUTTON_DOWN);

	state->clockwise_button = GrNewWindow(state->main_window,
					CLOCKWISE_BUTTON_X_POSITION,
					CLOCKWISE_BUTTON_Y_POSITION,
					CLOCKWISE_BUTTON_WIDTH,
					CLOCKWISE_BUTTON_HEIGHT, 0,
					BUTTON_BACKGROUND_COLOUR,
					0);
	GrSelectEvents(state->clockwise_button, GR_EVENT_MASK_EXPOSURE |
					GR_EVENT_MASK_BUTTON_DOWN);

	state->left_button = GrNewWindow(state->main_window,
					LEFT_BUTTON_X_POSITION,
					LEFT_BUTTON_Y_POSITION,
					LEFT_BUTTON_WIDTH,
					LEFT_BUTTON_HEIGHT, 0,
					BUTTON_BACKGROUND_COLOUR,
					0);
	GrSelectEvents(state->left_button, GR_EVENT_MASK_EXPOSURE |
					GR_EVENT_MASK_BUTTON_DOWN);

	state->right_button = GrNewWindow(state->main_window,
					RIGHT_BUTTON_X_POSITION,
					RIGHT_BUTTON_Y_POSITION,
					RIGHT_BUTTON_WIDTH,
					RIGHT_BUTTON_HEIGHT, 0,
					BUTTON_BACKGROUND_COLOUR,
					0);
	GrSelectEvents(state->right_button, GR_EVENT_MASK_EXPOSURE |
					GR_EVENT_MASK_BUTTON_DOWN);

	state->drop_button = GrNewWindow(state->main_window,
					DROP_BUTTON_X_POSITION,
					DROP_BUTTON_Y_POSITION,
					DROP_BUTTON_WIDTH,
					DROP_BUTTON_HEIGHT, 0,
					BUTTON_BACKGROUND_COLOUR,
					0);
	GrSelectEvents(state->drop_button, GR_EVENT_MASK_EXPOSURE |
					GR_EVENT_MASK_BUTTON_DOWN);

	state->well_window = GrNewWindow(state->main_window,
					WELL_WINDOW_X_POSITION,
					WELL_WINDOW_Y_POSITION,
					WELL_WINDOW_WIDTH,
					WELL_WINDOW_HEIGHT, 0,
					WELL_WINDOW_BACKGROUND_COLOUR, 0);
	GrSelectEvents(state->well_window, GR_EVENT_MASK_EXPOSURE);
	GrMapWindow(state->well_window);
	state->wellgc = GrNewGC();

	GrMapWindow(state->main_window);

	state->state = STATE_STOPPED;
	state->score = 0;
	state->level = 0;
	state->running_buttons_mapped = 0;

	srand15 (timer_milliseconds (uos_timer));

	choose_new_shape(state);
	new_game(state);
}

static void calculate_timeout(nstate *state)
{
	unsigned long msec;

	msec = timer_milliseconds (uos_timer);
	msec += delays [state->level];
	state->timeout = msec;
}

static void do_update(nstate *state)
{
	unsigned long msec;

	msec = timer_milliseconds (uos_timer);

	if (msec >= state->timeout) {
		drop_block_1(state);
		calculate_timeout(state);
	}
}

static void do_pause(nstate *state)
{
	draw_pause_continue_button(state);
	while(state->state == STATE_PAUSED) {
		GrGetNextEvent(&state->event);
		handle_event(state);
	}
	draw_pause_continue_button(state);
}

static void wait_for_start(nstate *state)
{
	draw_pause_continue_button(state);
	while(state->state == STATE_STOPPED) {
		GrGetNextEvent(&state->event);
		handle_event(state);
	}
	if(state->state == STATE_NEWGAME) state->state = STATE_RUNNING;
	draw_pause_continue_button(state);
	calculate_timeout(state);
}

static void run_game(nstate *state)
{
	while(state->state == STATE_RUNNING) {
		GrGetNextEvent(&state->event);
		handle_event(state);
		if(state->state == STATE_PAUSED) do_pause(state);
		if(state->state == STATE_RUNNING) do_update(state);
	}
}

void nxmain_ntetris (void *arg)
{
	static nstate game;

	init_game (&game);
	wait_for_start (&game);
	for (;;) {
		if (game.state == STATE_RUNNING)
			run_game (&game);

		if (game.state == STATE_STOPPED)
			wait_for_start (&game);

		if (game.state != STATE_EXIT)
			new_game (&game);
	}
}
