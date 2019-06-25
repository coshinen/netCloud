INC_DIR:= src
SRC_DIR:= src
INS_DIR:= /usr/local/bin
SRCS:= $(wildcard $(SRC_DIR)/*.c)
OBJS:= $(patsubst %.c, %.o, $(SRCS))
LIBS:= -lpthread -lcrypt -lcrypto -lmysqlclient

CC:= gcc

CFLAGS:= -std=c99 -w -g -Wno-deprecated $(addprefix -I, $(INC_DIR)) $(LIBS)

ELF:= ftpd

$(ELF):$(OBJS)
	$(CC) -o $(SRC_DIR)/$(ELF) $(OBJS) $(CFLAGS)
	cd src/cli; make

install:
	install -m 0755 $(SRC_DIR)/$(ELF) $(INS_DIR)
	cd src/cli; make install

uninstall:
	rm -rf $(INS_DIR)/$(ELF)
	cd src/cli; make uninstall

clean:
	rm -rf $(SRC_DIR)/$(ELF)
	rm -rf $(OBJS)
	cd src/cli; make clean
