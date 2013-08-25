#ifndef YUV_FRAME_H
#define YUV_FRAME_H

#include <cstdint>
#include <vector>

namespace video_utils {

    class YUVFrame {
        public:
            YUVFrame() {}
            YUVFrame(int width, int height) : width_(width), height_(height),
                y_(width * height), u_(width * height / 4), v_(width * height / 4) {}

            void CopyYPlane(std::vector<uint8_t>&& y) { y_ = y; }
            void CopyUPlane(std::vector<uint8_t>&& u) { u_ = u; }
            void CopyVPlane(std::vector<uint8_t>&& v) { v_ = v; }
            
            uint8_t getY(int x, int y) const { return y_[x + y * width_]; }
            uint8_t getU(int x, int y) const { return u_[x / 2 + y / 2 * width_]; }
            uint8_t getV(int x, int y) const { return v_[x / 2 + y / 2 * width_]; }
            void setY(int x, int y, uint8_t value) { y_[x + y * width_] = value; }
            void setU(int x, int y, uint8_t value) { u_[x / 2 + y / 2 * width_] = value; }
            void setV(int x, int y, uint8_t value) { v_[x / 2 + y / 2 * width_] = value; }

            int width() const { return width_; }
            int height() const { return height_; }
            void set_width(int width) { width_ = width; }
            void set_height(int height) { height_ = height; }

        private:
            int width_;
            int height_;
            std::vector<uint8_t> y_;
            std::vector<uint8_t> u_;
            std::vector<uint8_t> v_;
    };
} // namespace video_utils
#endif // YUV_FRAME_H
