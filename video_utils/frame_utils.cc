#include "frame_utils.h"

namespace video_utils {
    using namespace std;

    namespace {

        class CRCGenerator {
            public:
                CRCGenerator() : crc_table(256) {
                    uint32_t remainder;

                    for (int dividend = 0; dividend < 256; dividend++) {
                        remainder = static_cast<uint32_t>(dividend) << 24;
                        for (int k = 0; k < 8; k++) {
                            if (remainder & 0x80000000) {
                                remainder = 0x04c11db7 ^ (remainder << 1);
                            } else {
                                remainder <<= 1;
                            }
                        }
                        crc_table[dividend] = remainder;
                    }
                }

                void Update(vector<uint8_t> data) {
                    for (int n = 0; n < static_cast<int>(data.size()); n++) {
                        const int i = ((current_crc >> 24) ^ data[n]) & 0xff;
                        current_crc = crc_table[i] ^ (current_crc << 8);
                    }
                    current_crc = ~current_crc;
                }

                void Update(uint8_t data) {
                    Update(vector<uint8_t>(1, data));
                }

                void Update(uint32_t data) {
                    for (int i = 0; i < 4; i++) {
                        Update(static_cast<uint8_t>(data >> 8 * i));
                    }
                }

                uint32_t Get() {
                    uint32_t ret_val = current_crc;
                    current_crc = 0xffffffff;
                    return ret_val;
                }

            private:
                vector<uint32_t> crc_table;
                uint32_t current_crc = 0xffffffff;
        };

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

    YUVFrame ConvertRGBToYUV(const RGBFrame& rgb_frame) {
        const int width = rgb_frame.width();
        const int height = rgb_frame.height();
        YUVFrame yuv_frame(width, height);

        for (int j = 0; j < height; j++) {
            for (int i = 0; i < width; i++) {
                const uint8_t r = rgb_frame.getR(i, j);
                const uint8_t g = rgb_frame.getG(i, j);
                const uint8_t b = rgb_frame.getB(i, j);
                const uint8_t y = 0.299f * r + 0.587f * g + 0.114f * b;
                const uint8_t u = -0.169f * r - 0.331f * g + 0.499f * b + 128;
                const uint8_t v = 0.499f * r - 0.418f * g - 0.0813f * b + 128;
                yuv_frame.setY(i, j, y);
                yuv_frame.setU(i, j, u);
                yuv_frame.setV(i, j, v);
            }
        }

        return yuv_frame;
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
