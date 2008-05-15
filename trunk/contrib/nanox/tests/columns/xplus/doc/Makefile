SHELL   = /bin/sh

all: guide2.ps

guide.dvi: guide.tex
	tex guide.tex

guide.ps: guide.dvi
	dvips -Z guide.dvi -o guide.ps

guide2.ps: mkbilist guide.ps
	mkbilist -a4 -o `mkorder guide.ps` guide.ps > guide2.ps

cursor.a: /usr/lib/X11/fonts/misc/cursor.snf.Z
	zcat /usr/lib/X11/fonts/misc/cursor.snf.Z > cursor.snf
	showsnf -g cursor.snf > cursor.a
	rm cursor.snf

cursor.mf: /usr/lib/X11/fonts/misc/cursor.snf.Z snf2mf
	zcat /usr/lib/X11/fonts/misc/cursor.snf.Z > cursor.snf
	showsnf -g cursor.snf | sed '/^character/,$$!d' | snf2mf > cursor.mf
	rm cursor.snf

cursor.tfm: cursor.mf
	mf '\mode:=imagen; mag:=magstep(0); input' cursor.mf
	gftopk cursor.300gf
	rm cursor.300gf

install: cursor.tfm
	cp cursor.tfm /usr/local/lib/tex/fonts/tfm
	cp cursor.300pk /usr/local/lib/tex/fonts/pk
