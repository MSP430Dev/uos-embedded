CC_HOST = gcc

# Для использования сокетов TCP, иначе собирается версия с UDP
CFLAGS  = -DTCP_SOCKET

LDFLAGS	=
LIBS 	= 
EXECS	= server client client-2way

.PHONY: all clean 

all:
	$(CC_HOST) server.c $(CFLAGS) $(LDFLAGS) -Wl,-rpath=./ $(addprefix -l,$(LIBS)) -o server
	$(CC_HOST) client.c $(CFLAGS) $(LDFLAGS) -Wl,-rpath=./ $(addprefix -l,$(LIBS)) -o client
	$(CC_HOST) client-2way.c $(CFLAGS) $(LDFLAGS) -Wl,-rpath=./ $(addprefix -l,$(LIBS)) -o client-2way

clean: 
	rm -f $(EXECS)

