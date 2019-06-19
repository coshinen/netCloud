SOURCES:=$(wildcard *.c)
OBJS:=$(patsubst %.c,%.o,$(SOURCES))
CFLAGS:= -I .
ftpd:$(OBJS)
	cd src; gcc $^ -o $@ -pthread -lcrypt -lcrypto -lmysqlclient
	cd cli; make
clean:
	rm -rf $(OBJS) ftpd
