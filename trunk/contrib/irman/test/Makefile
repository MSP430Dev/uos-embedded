include ../config

SREC		= code.sre
CFLAGS		+= -DKHZ=4000
OBJS		= lcd.o motor.o irman.o
PROGR		= megaprog -f /dev/ttyS0

all:		$(SREC)
		avr-size *.out

clean:
		rm -f *.out *.hex *.[osi] *.lst *~

code.out:	$(OBJS) ../lib/libuos.a main.c
		$(CC) $(CFLAGS) -c main.c -o $*.o
		$(CC) $(LDFLAGS) $*.o $(OBJS) $(LIBS) -o $@
		@rm -f $*.o
#		avr-objdump -D $@ > $*.dis

load:
		$(PROGR) $(SREC)

depend:
		@cp Makefile Makefile~~
		(sed '/^###/,$$d' Makefile;\
		echo '### DO NOT DELETE THIS LINE';\
		$(CC) -MM $(CFLAGS) *.c;\
		echo '# DEPENDENCIES MUST END AT END OF FILE';\
		echo '# IF YOU PUT STUFF HERE IT WILL GO AWAY';\
		echo '# see make depend above') > Makefile~ &&\
		mv Makefile~ Makefile

### DO NOT DELETE THIS LINE
