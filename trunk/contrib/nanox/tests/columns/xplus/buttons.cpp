# include "window.h"
# include "button.h"
# include "panel.h"

main (int argc, char **argv)
{
	xPanel *panel;

	xScreen *screen = new xScreen ("Foo", &argc, argv);

	panel = new xPanel (screen, "panel");
	new xButton (panel, "but1");
	new xButton (panel, "but2");
	new xButton (panel, "but3");
	new xButton (panel, "but4");

	screen->Run ();
	return (0);
}
