SRC=../src
INC=../include
CXX=g++
CFLAGS=-O -std=c++11 -Wall -Wno-unused-result -Wno-deprecated-register -I$(INC)
LIB=

MONGOOSE_SRC=$(INC)/mongoose/mongoose.c
MSL_SRC=$(INC)/msl/serial.cpp $(INC)/msl/time.cpp

ifeq ($(OS),Windows_NT)
	LIB+=-lWs2_32
endif

ifeq ($(shell uname),Darwin)
	LIB+=-framework IOKit -framework CoreFoundation
endif

all: backend frontend

backend: $(SRC)/backend.cpp $(SRC)/network.cpp $(MONGOOSE_SRC) $(MSL_SRC)
	$(CXX) $(CFLAGS) $(LIB) $^ -o $@

frontend: $(SRC)/frontend.cpp $(SRC)/network.cpp $(MONGOOSE_SRC) $(MSL_SRC)
	$(CXX) $(CFLAGS) $(LIB) $^ -o $@

clean:
	-rm -rf backend backend.exe frontend frontend.exe
