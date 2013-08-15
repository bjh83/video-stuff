#ifndef PNG_WRITER_H
#define PNG_WRITER_H

#include <fstream>
#include <string>
#include "rgb_frame.h"

namespace video_utils {
    class CRCGenerator {
        public:
            CRCGenerator();
            void Update(const std::vector<uint8_t>& data);
            void Update(uint8_t data);
            void Update(uint32_t data);
            uint32_t Get();

        private:
            std::vector<uint32_t> crc_table;
            uint32_t current_crc = 0xffffffff;
    };

    class PNGWriter {
        public:
            PNGWriter();

            bool Write(const std::string& file_name, const RGBFrame& rgb_frame);

        private:
            std::fstream file_;
            CRCGenerator crc_generator_;
            bool WriteHeader(int width, int height);
            bool WriteFrame(const RGBFrame& rgb_frame);
            bool WriteEnd();
    };
} // namespace video_utils
#endif // PNG_WRITER_H
