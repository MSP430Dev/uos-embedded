# include "window.h"
# include "label.h"
# include "panel.h"

main (int argc, char **argv)
{
	xPanel *panel;

	xScreen *screen = new xScreen ("Foo", &argc, argv);

	panel = new xPanel (screen, 1);
	new xLabel (panel, "Amet");
	new xLabel (panel, "Berta");
	new xLabel (panel, "Relcom");
	new xLabel (panel, "Quit");

	screen->Run ();
	return (0);
}
