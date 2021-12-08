CC=g++
CFLAGS_DEBUG=-Wall -pedantic -g -DLOG_USE-COLOR -DDEBUG
LIBS = -lsimlib -lm
TARGET = ims
SOURCES=$(wildcard src/*.cpp)
OBJECTS=$(SOURCES:%.cpp=%.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $^ $(CFLAGS_DEBUG) -o $@ $(LIBS)

.cpp.o:
	$(CC) -c $(CFLAGS_DEBUG) $< -o $@ $(LIBS)

run:
	./ims

clean:
	rm src/*.o
	rm $(TARGET)
	rm model.out

