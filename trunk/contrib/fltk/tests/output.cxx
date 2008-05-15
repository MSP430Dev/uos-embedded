#include <FL/Fl.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Double_Window.H>
#include <FL/fl_ask.H>
#include <FL/Fl_File_Chooser.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Return_Button.H>
#include <FL/Fl_Text_Buffer.H>
#include <FL/Fl_Text_Editor.H>

#include <runtime/lib.h>
#include <kernel/uos.h>
#include <stream/stream.h>
#include <stream/pipe.h>

int changed = 0;
int loading = 0;
char filename [256] = "";
Fl_Text_Buffer *textbuf = 0;

extern int fl_disable_transient_for;

void save_cb (void);
void saveas_cb (void);

extern "C" {
	void output_main (void *arg);
};

class OutputWindow : public Fl_Double_Window {
public:
	OutputWindow (int w, int h, const char *t, pipe_t *pipe);
	~OutputWindow ();

	Fl_Text_Display *output;
	pipe_t *pipe;
	int handle (int event);
	void put_char (int ch);
};

OutputWindow::OutputWindow (int w, int h, const char *t, pipe_t *p)
	: Fl_Double_Window (w, h, t)
{
	output = 0;
	pipe = p;
}

OutputWindow::~OutputWindow ()
{
}

void OutputWindow::put_char (int ch)
{
	char buf [20];

	if (ch == '\b') {
		int len = textbuf->length();
		if (len <= 0)
			return;
		output->show_cursor (0);
		textbuf->remove (len-1, len);
		goto show;
	}

	if (ch <= '~') {
		buf[0] = ch;
		buf[1] = 0;
	} else {
		snprintf (buf, sizeof(buf), "<%x>", ch);
	}
	output->show_cursor (0);
	textbuf->append (buf);
show:
	output->insert_position (textbuf->length());
	output->show_insert_position ();
	output->show_cursor (1);
	::changed = 1;
}

int OutputWindow::handle (int event)
{
        while (peekchar (&pipe->master) >= 0)
		put_char (getchar (&pipe->master));

	if (output && event == FL_KEYBOARD &&
	    ! (Fl::event_state() & (FL_CTRL | FL_COMMAND))) {
		unsigned ch = Fl::event_key ();

		switch (ch) {
		case FL_BackSpace:
			ch = '\b';
			break;

		case FL_Tab:
			ch = '\t';
			break;

		case FL_Enter:
		case FL_KP_Enter:
			ch = '\n';
			break;

		case FL_Escape:
			ch = '\e';
			break;
		}
		putchar (&pipe->master, ch);
		return 1;
	}
	return Fl_Double_Window::handle (event);
}

int check_save (void)
{
	if (! changed)
		return 1;

	int r = fl_choice ("The current output has not been saved.\n"
	    "Would you like to save it now?",
	    "Cancel", "Save", "Discard");

	if (r == 1) {
		save_cb ();
		// Save the file...
		return ! changed;
	}
	return (r == 2) ? 1 : 0;
}

void save_file (char *newfile)
{
	if (textbuf->savefile (newfile))
		fl_alert ("Error writing to file \'%s\'.", newfile);
	else
		strcpy (filename, newfile);
	changed = 0;
	textbuf->call_modify_callbacks ();
}

void changed_cb (int, int nInserted, int nDeleted, int, const char *, void *v)
{
	if ((nInserted || nDeleted) && !loading)
		changed = 1;
	OutputWindow *w = (OutputWindow *) v;

	if (loading)
		w->output->show_insert_position ();
}

void new_cb (Fl_Widget *, void *)
{
	if (! check_save ())
		return;

	filename[0] = '\0';
	textbuf->select (0, textbuf->length ());
	textbuf->remove_selection ();
	changed = 0;
	textbuf->call_modify_callbacks ();
}

void close_cb (Fl_Widget *, void *v)
{
	Fl_Window *w = (Fl_Window *) v;

	if (! check_save ())
		return;

	w->hide ();
	textbuf->remove_modify_callback (changed_cb, w);
	delete w;
}

void quit_cb (Fl_Widget *, void *)
{
	if (changed && !check_save ())
		return;

	exit (0);
}

void save_cb ()
{
	if (filename[0] == '\0') {
		//No filename - get one !
		saveas_cb ();
		return;
	} else
		save_file (filename);
}

void saveas_cb ()
{
	char *newfile;

	newfile = fl_file_chooser ("Save Output As?", "*", filename);
	if (newfile != NULL)
		save_file (newfile);
}

Fl_Menu_Item menuitems[] = {
	{"&File", 0, 0, 0, FL_SUBMENU},
	{"&Clear", 0, (Fl_Callback *) new_cb},
	{"&Save", FL_CTRL + 's', (Fl_Callback *) save_cb},
	{"Save &As...", FL_CTRL + FL_SHIFT + 's', (Fl_Callback *) saveas_cb, 0, FL_MENU_DIVIDER},
	{"E&xit", FL_CTRL + 'q', (Fl_Callback *) quit_cb, 0},
	{0},

	{0}
};

Fl_Window *new_view (pipe_t *pipe)
{
	OutputWindow *w = new OutputWindow (660, 400, "Tcl", pipe);
	w->label ("Tcl");

	w->begin ();
	Fl_Menu_Bar *m = new Fl_Menu_Bar (0, 0, 660, 30);

	m->copy (menuitems, w);
	w->output = new Fl_Text_Display (0, 30, 660, 370);
	w->output->buffer (textbuf);
	w->output->textfont (FL_COURIER);
	w->output->cursor_style (Fl_Text_Display::BLOCK_CURSOR);
	w->output->show_cursor (1);
	w->end ();
	w->resizable (w->output);
	w->callback ((Fl_Callback *) close_cb, w);

	textbuf->add_modify_callback (changed_cb, w);
	textbuf->call_modify_callbacks ();
	return w;
}

void output_main (void *arg)
{
	char *argv[] = {"tcl", 0};
	pipe_t *pipe = (pipe_t*) arg;
	extern int fl_disable_transient_for;

	FL_NORMAL_SIZE = 17;
	fl_disable_transient_for = 1;

	textbuf = new Fl_Text_Buffer;

	Fl_Window *window = new_view (pipe);

	window->show (1, argv);

	Fl::run ();

	delete textbuf;
	fclose (&pipe->master);
	task_exit (0);
}
