#include "yuv_reader.h"

#include <algorithm>
#include <vector>

namespace video_utils {
    using namespace std;
    bool YUVReader::ReadFrame(YUVFrame& video_frame) {
        const int y_frame_size = width_ * height_;
        const int u_frame_size = y_frame_size / 4;
        const int v_frame_size = y_frame_size / 4;
        vector<uint8_t> y(y_frame_size);
        vector<uint8_t> u(u_frame_size);
        vector<uint8_t> v(v_frame_size);

        file.read((char*)y.data(), y_frame_size);
        if (!file) {
            return false;
        }
        file.read((char*)u.data(), u_frame_size);
        if (!file) {
            return false;
        }
        file.read((char*)v.data(), v_frame_size);
        if (!file) {
            return false;
        }

        video_frame.set_width(width_);
        video_frame.set_height(height_);
        video_frame.CopyYPlane(move(y));
        video_frame.CopyUPlane(move(u));
        video_frame.CopyVPlane(move(v));
        return true;
    }
}
