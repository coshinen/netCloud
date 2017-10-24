SOURCES:=$(wildcard *.c)
OBJS:=$(patsubst %.c,%.o,$(SOURCES))
CFLAGS:= -I .
ftpc:$(OBJS)
	gcc $^ -o $@ -lcrypt -lcrypto -pthread
clean:
	rm -rf $(OBJS) ftpc
