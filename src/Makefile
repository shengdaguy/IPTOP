ROOT=.
JSON=$(ROOT)/jsoncpp

CC = gcc
CXX = g++
INC = -I./ -I./jsoncpp/
LIB = -lstdc++ -lpthread -lpcap -std=c++11 -lcurl
CFLAGS = -g -Wall -D_REENTRANT 
#CFLAGS = -O3 -Wall -D_REENTRANT -std=c++11 

BIN = iptop

all: $(BIN)

JSONS = $(JSON)/json_value.o \
		$(JSON)/json_reader.o \
		$(JSON)/json_writer.o

OBJS = main.o \
	   global.o \
	   capture.o \
	   util.o \
	   init.o \
	   report.o \

$(BIN): $(OBJS) $(JSONS)
	$(CXX) -o $@ $^ $(LIB) $(CFLAGS)

%.o: %.c
	$(CXX) -c -o $@ $< $(INC) $(CFLAGS)

%.o: %.cpp
	$(CXX) -c -o $@ $< $(INC) $(CFLAGS)

clean:
	rm -f $(BIN) *.o $(JSON)/*.o 






