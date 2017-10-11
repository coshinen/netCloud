SOURCES:=$(wildcard *.c)
OBJS:=$(patsubst %.c,%.o,$(SOURCES))
CFLAGS:= -I ../include
ftpd:$(OBJS)
	gcc $^ -o $@ -pthread -lcrypt -lcrypto -lmysqlclient
clean:
	rm -rf $(OBJS) ftpd
