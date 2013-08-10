#ifndef RGB_FRAME_H
#define RGB_FRAME_H

#include <cstdint>
#include <vector>

namespace video_utils {

    class RGBFrame {
        public:
            RGBFrame() {}
            RGBFrame(int width, int height) : width_(width), height_(height),
                r_(width * height), g_(width * height), b_(width * height) {}

            void CopyRPlane(std::vector<uint8_t>&& r) { r_ = r; }
            void CopyGPlane(std::vector<uint8_t>&& g) { g_ = g; }
            void CopyBPlane(std::vector<uint8_t>&& b) { b_ = b; }

            uint8_t getR(int x, int y) const { return r_[x + y * width_]; }
            uint8_t getG(int x, int y) const { return g_[x + y * width_]; }
            uint8_t getB(int x, int y) const { return b_[x + y * width_]; }
            void setR(int x, int y, uint8_t value) { r_[x + y * width_] = value; }
            void setG(int x, int y, uint8_t value) { g_[x + y * width_] = value; }
            void setB(int x, int y, uint8_t value) { b_[x + y * width_] = value; }

            int width() const { return width_; }
            int height() const { return height_; }
            void set_width(int width) { width_ = width; }
            void set_height(int height) { height_ = height; }

        private:
            int width_;
            int height_;
            std::vector<uint8_t> r_;
            std::vector<uint8_t> g_;
            std::vector<uint8_t> b_;
    };
} // namespace video_utils
#endif // RGB_FRAME_H
