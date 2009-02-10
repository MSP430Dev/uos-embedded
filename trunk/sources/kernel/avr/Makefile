ROUTINES	= ATmega103.c

all:		$(ROUTINES)

clean:
		rm -f *~

$(ROUTINES):	mkintr.py interrupt.cfg
		./mkintr.py interrupt.cfg
