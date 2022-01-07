CFLAGS = -D _DEBUG -lstdc++ -lm -std=c++0x -I../../include -I/usr/local/include -L../../lib -L/lib64 -L/usr/local/lib  -g  -Wall
CC=g++
SRC=$(wildcard *.cpp)
OBJ=$(addprefix ./, $(addsuffix .o, $(basename $(SRC))))
TARGET=init_test

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) -o $@  $^ $(CFLAGS)  

clean:
	rm -f $(TARGET) $(OBJ)   
