#include <iostream>
#include "video_utils/rgb_frame.h"
#include "video_utils/png_writer.h"

using namespace std;
using video_utils::PNGWriter;
using video_utils::RGBFrame;

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cout << "Please provide just name of file to write.";
        return 0;
    }

    const int width = 400;
    const int height = 400;
    RGBFrame rgb_frame(width, height);

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            rgb_frame.setR(x, y, 255);
            rgb_frame.setG(x, y, 255);
            rgb_frame.setB(x, y, 255);
        }
    }

    PNGWriter writer;
    writer.Write(argv[1], rgb_frame);
    return 0;
}
