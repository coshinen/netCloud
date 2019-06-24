INC_DIR:= .
SRC_DIR:= .
DIST_DIR:= ..
INST_DIR:= /usr/local/bin
SRCS:= $(wildcard $(SRC_DIR)/*.c)
OBJS:= $(patsubst %.c, %.o, $(SRCS))

CXX:= gcc

CXXFLAGS:= -std=c99 $(addprefix -I, $(INC_DIR)) -pthread -lcrypt -lcrypto

EXE:= ftp-cli

$(EXE):$(OBJS)
	$(CXX) -o $(SRC_DIR)/$(DIST_DIR)/$(EXE) $(OBJS) $(CXXFLAGS)

install:
	install -m 0755 $(SRC_DIR)/$(DIST_DIR)/$(EXE) $(INST_DIR)

uninstall:
	rm -rf $(INST_DIR)/$(EXE)

clean:
	rm -rf $(SRC_DIR)/$(DIST_DIR)/$(EXE)
	rm -rf $(OBJS)
