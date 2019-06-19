INC_DIR:= src
SRC_DIR:= src
INS_DIR:= /usr/local/bin
SRCS:= $(wildcard $(SRC_DIR)/*.cc)
OBJS:= $(patsubst %.cc, %.o, $(SRCS))

CXX:= gcc

CXXFLAGS:= $(addprefix -I, $(INC_DIR)) -pthread -lcrypt -lcrypto -lmysqlclient

EXE:= ftps

$(EXE):$(OBJS)
	$(CXX) -o $(SRC_DIR)/$(EXE) $(OBJS) $(CXXFLAGS)
	cd cli; make

install:
	install -m 0755 $(SRC_DIR)/$(EXE) $(INS_DIR)
	cd cli; install

uninstall:
	rm -rf $(INS_DIR)/$(EXE)
	cd cli; uninstall

clean:
	rm -rf $(SRC_DIR)/$(EXE)
	rm -rf $(OBJS)
	cd cli; clean
