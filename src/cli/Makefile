INC_DIR:= .
SRC_DIR:= .
DIST_DIR:= ..
INST_DIR:= /usr/local/bin
SRCS:= $(wildcard $(SRC_DIR)/*.c)
OBJS:= $(patsubst %.c, %.o, $(SRCS))

CC:= gcc

CFLAGS:= -std=c99 $(addprefix -I, $(INC_DIR)) -pthread -lcrypt -lcrypto

ELF:= ftp-cli

$(ELF):$(OBJS)
	$(CC) -o $(SRC_DIR)/$(DIST_DIR)/$(ELF) $(OBJS) $(CFLAGS)

install:
	install -m 0755 $(SRC_DIR)/$(DIST_DIR)/$(ELF) $(INST_DIR)

uninstall:
	rm -rf $(INST_DIR)/$(ELF)

clean:
	rm -rf $(SRC_DIR)/$(DIST_DIR)/$(ELF)
	rm -rf $(OBJS)
