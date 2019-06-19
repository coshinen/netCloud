INC_DIR:= .
SRC_DIR:= .
INS_DIR:= /usr/local/bin
SRCS:= $(wildcard $(SRC_DIR)/*.cc)
OBJS:= $(patsubst %.cc, %.o, $(SRCS))

CXX:= gcc

CXXFLAGS:= $(addprefix -I, $(INC_DIR)) -pthread -lcrypt -lcrypto

EXE:= ftpc

$(EXE):$(OBJS)
	$(CXX) -o $(SRC_DIR)/$(EXE) $(OBJS) $(CXXFLAGS)

install:
	install -m 0755 $(SRC_DIR)/$(EXE) $(INS_DIR)

uninstall:
	rm -rf $(INS_DIR)/$(EXE)

clean:
	rm -rf $(SRC_DIR)/$(EXE)
	rm -rf $(OBJS)
