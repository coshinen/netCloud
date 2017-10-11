SOURCES:=$(wildcard *.c)
OBJS:=$(patsubst %.c,%.o,$(SOURCES))
CFLAGS:= -I .
ftpc:$(OBJS)
	gcc $^ -o $@ -lcrypt -lcrypto
clean:
	rm -rf $(OBJS) ftpc
