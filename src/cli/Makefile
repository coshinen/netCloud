INC_DIR:= .
SRC_DIR:= .
DIST_DIR:= ..
INST_DIR:= /usr/local/bin
SRCS:= $(wildcard $(SRC_DIR)/*.c)
OBJS:= $(patsubst %.c, %.o, $(SRCS))

CC:= gcc

CFLAGS:= $(addprefix -I, $(INC_DIR)) -pthread -lcrypt -lcrypto

EXE:= ftp-cli

$(EXE):$(OBJS)
	$(CC) -o $(SRC_DIR)/$(DIST_DIR)/$(EXE) $(OBJS) $(CFLAGS)

install:
	install -m 0755 $(SRC_DIR)/$(DIST_DIR)/$(EXE) $(INST_DIR)

uninstall:
	rm -rf $(INST_DIR)/$(EXE)

clean:
	rm -rf $(SRC_DIR)/$(DIST_DIR)/$(EXE)
	rm -rf $(OBJS)
