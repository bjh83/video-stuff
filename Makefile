PROJECT= white_write
CC= g++
OBJS= main.o png_writer.o
CXXFLAGS= -std=c++11

.PHONY: all

all: $(PROJECT)

$(PROJECT): $(OBJS)
	$(CC) -o $(PROJECT) $(OBJS)

$(OBJS): video_utils/rgb_frame.h video_utils/png_writer.h video_utils/png_writer.cc

.PHONY: clean

clean:
	rm -rf $(OBJS) $(PROJECT)
