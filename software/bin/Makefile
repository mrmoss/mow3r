SRC=../src
INC=../include
CXX=g++
CFLAGS=-O -std=c++11 -Wall -Wno-unused-result -Wno-deprecated-register -I$(INC)
LIB=-lcrypto

JSONCPP=$(INC)/jsoncpp/json_reader.cpp $(INC)/jsoncpp/json_value.cpp $(INC)/jsoncpp/json_writer.cpp
MONGOOSE=$(INC)/mongoose/mongoose.c
MSL=$(INC)/msl/crypto.cpp $(INC)/msl/serial.cpp $(INC)/msl/time.cpp

ifeq ($(OS),Windows_NT)
	LIB+=-lWs2_32 -lgdi32
endif

ifeq ($(shell uname),Darwin)
	LIB+=-framework IOKit -framework CoreFoundation
endif

all: backend

backend: $(SRC)/backend.cpp $(SRC)/network.cpp $(JSONCPP) $(MONGOOSE) $(MSL)
	$(CXX) $(CFLAGS) $(LIB) $^ -o $@

clean:
	-rm -rf backend backend.exe
