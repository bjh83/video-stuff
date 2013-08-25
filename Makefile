PROJECT= white_write
CC= g++
OBJS= main.o video_utils/png_writer.o utils/file.o
CXXFLAGS= -std=c++11 -I/home/brendan

.PHONY: all

all: $(PROJECT)

$(PROJECT): $(OBJS)
	$(CC) -o $(PROJECT) $(OBJS)

$(OBJS): video_utils/rgb_frame.h video_utils/png_writer.h utils/file.h utils/stream_interface.h

.PHONY: clean

clean:
	rm -rf $(OBJS) $(PROJECT)
