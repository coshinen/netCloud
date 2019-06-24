INC_DIR:= src
SRC_DIR:= src
INS_DIR:= /usr/local/bin
SRCS:= $(wildcard $(SRC_DIR)/*.c)
OBJS:= $(patsubst %.c, %.o, $(SRCS))

CC:= gcc

CFLAGS:= -std=c99 $(addprefix -I, $(INC_DIR)) -pthread -lcrypt -lcrypto -lmysqlclient

EXE:= ftpd

$(EXE):$(OBJS)
	$(CC) -o $(SRC_DIR)/$(EXE) $(OBJS) $(CFLAGS)
	cd src/cli; make

install:
	install -m 0755 $(SRC_DIR)/$(EXE) $(INS_DIR)
	cd src/cli; make install

uninstall:
	rm -rf $(INS_DIR)/$(EXE)
	cd src/cli; make uninstall

clean:
	rm -rf $(SRC_DIR)/$(EXE)
	rm -rf $(OBJS)
	cd src/cli; make clean
