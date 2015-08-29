TARGET := tsparser

CC     := gcc
CFLAGS := -Wall -O2 -g
LDFLAGS+= -lstdc++
SOURCE := $(wildcard *.cpp)
OBJS   := $(patsubst %.cpp, %.o, $(SOURCE))

all: $(TARGET)
$(TARGET) : $(OBJS)
	@$(CC) $(CFLAGS) $(LDFLAGS) $(OBJS) -o $(TARGET)

.PHONY: clean

clean:
	@rm -f $(TARGET) $(OBJS) *.o
