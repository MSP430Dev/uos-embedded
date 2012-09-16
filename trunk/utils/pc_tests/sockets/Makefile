CC_HOST = gcc

# Для использования сокетов TCP, иначе собирается версия с UDP
CFLAGS  = -DTCP_SOCKET

LDFLAGS	=
LIBS 	= 
EXECS	= server client

.PHONY: all clean 

all:
	$(CC_HOST) server.c $(CFLAGS) $(LDFLAGS) -Wl,-rpath=./ $(addprefix -l,$(LIBS)) -o server
	$(CC_HOST) client.c $(CFLAGS) $(LDFLAGS) -Wl,-rpath=./ $(addprefix -l,$(LIBS)) -o client

clean: 
	rm -f $(EXECS)

