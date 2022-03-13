CC=g++
CFLAGS=-I. -std=c++11
TARGET=ida

$(TARGET): $(TARGET).cpp
	$(CC) -o $@ $^ $(CFLAGS)

debug: CFLAGS += -g
debug: $(TARGET)

optim: CFLAGS += -O3
optim: $(TARGET)
