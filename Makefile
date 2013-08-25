PROJECT= white_write
CC= g++
OBJS= main.o png_writer.o
CXXFLAGS= -std=c++11

.PHONY: all

all: $(PROJECT)

$(PROJECT): $(OBJS)
	$(CC) -o $(PROJECT) $(OBJS)

$(OBJS): rgb_frame.h png_writer.h

.PHONY: clean

clean:
	rm -rf $(OBJS) $(PROJECT)
