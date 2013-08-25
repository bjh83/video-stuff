#ifndef YUV_READER_H
#define YUV_READER_H

#include <fstream>
#include <string>
#include "yuv_frame.h"

namespace video_utils {

class YUVReader {
    public:
        YUVReader(std::string file_name, int width, int height) :
            width_(width), height_(height), file(file_name,
            std::fstream::in | std::fstream::binary) {}
        bool ReadFrame(YUVFrame& frame);
        bool eof() { return file.eof(); }
    private:
        int width_;
        int height_;
        int offset_ = 0;
        std::fstream file;
};
} // namespace video_utils
#endif // YUV_READER_H
