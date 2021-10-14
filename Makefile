CC=g++
CFLAGS_DEBUG=-Wall -pedantic -g -fsanitize=leak -fsanitize=address -DLOG_USE-COLOR -DDEBUG
TARGET = ims
SOURCES=$(wildcard src/*.cpp)
OBJECTS=$(SOURCES:%.cpp=%.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $^ $(CFLAGS_DEBUG) -o $@

.cpp.o:
	$(CC) -c $(CFLAGS_DEBUG) $< -o $@

clean:
	rm src/*.o
	rm $(TARGET)

