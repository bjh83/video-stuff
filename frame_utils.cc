#include "frame_utils.h"

namespace video_utils {
    using namespace std;

    namespace {

        uint8_t clamp(float value) {
            if (value < 0) {
                return 0;
            }
            if (value > 255) {
                return 255;
            }
            return static_cast<uint8_t>(value);
        }
    } // namespace

    RGBFrame ConvertYUVToRGB(const YUVFrame& yuv_frame) {
        const int width = yuv_frame.width();
        const int height = yuv_frame.height();
        RGBFrame rgb_frame(width, height);

        for (int j = 0; j < height; j++) {
            for (int i = 0; i < width; i++) {
                const uint8_t y = yuv_frame.getY(i, j);
                const uint8_t u = yuv_frame.getU(i, j);
                const uint8_t v = yuv_frame.getV(i, j);
                const uint8_t r = clamp(y + 1.402f * (v - 128));
                const uint8_t g = clamp(y - 0.344f * (u - 128) - 0.714f * (v - 128));
                const uint8_t b = clamp(y + 1.772f * (u - 128));
                rgb_frame.setR(i, j, r);
                rgb_frame.setG(i, j, g);
                rgb_frame.setB(i, j, b);
            }
        }

        return rgb_frame;
    }

    bool WritePNG(string file_base_name, const RGBFrame& rgb_frame) {
        const vector<uint8_t> magic_numbers = {137, 80, 78, 71, 13, 10, 26, 10};
        const uint32_t ihdr_length = 13;
        const vector<uint8_t> ihdr_message = {73, 72, 68, 82};
        const uint32_t width = rgb_frame.width();
        const uint32_t height = rgb_frame.height();
        const uint8_t bit_depth = 8;
        const uint8_t color_type = 2;
        const uint8_t compression_method = 0;
        const uint8_t filter_method = 0;
        const uint8_t interlace_method = 0;
    }
} // namespace video_utils
