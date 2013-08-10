#ifndef FRAME_UTILS_H
#define FRAME_UTILS_H

#include <string>
#include "rgb_frame.h"
#include "yuv_frame.h"

namespace video_utils {

    RGBFrame ConvertYUVToRGB(const YUVFrame& yuv_frame);

    bool WritePNG(std::string file_base_name, const RGBFrame& rgb_frame);
} // namespace video_utils
#endif // FRAME_UTILS_H
