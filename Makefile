CC=g++
CFLAGS_DEBUG=-Wall -pedantic -g -DLOG_USE-COLOR -DDEBUG
CFLAGS=-O3
LIBS = -lsimlib -lm
TARGET = ims
SOURCES=$(wildcard src/*.cpp)
OBJECTS=$(SOURCES:%.cpp=%.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $^ $(CFLAGS) -o $@ $(LIBS)

.cpp.o:
	$(CC) -c $(CFLAGS) $< -o $@ $(LIBS)

run:
	./ims 3 1 1 1 2 1 4 2 6
	# ./ims 6 1 1 1 4 1 4 4 6
	# ./ims 12 1 2 1 8 1 4 8 6
	# ./ims 24 1 2 1 16 1 8 16 6

clean:
	rm src/*.o
	rm $(TARGET)
	rm model.out

zip:
	zip -r 10_xhrmor00_xdokou14.zip src Makefile dokumentace.pdf