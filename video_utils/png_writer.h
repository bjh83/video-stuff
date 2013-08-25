#ifndef PNG_WRITER_H
#define PNG_WRITER_H

#include <fstream>
#include <string>
#include "rgb_frame.h"
#include "video-stuff/utils/file.h"

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

    class AdlerGenerator {
        public:
            AdlerGenerator() {}
            void Update(const std::vector<uint8_t>& data);
            void Update(uint8_t data);
            void Update(uint32_t data);
            uint32_t Get();

        private:
            uint32_t a_ = 1;
            uint32_t b_ = 0;
    };

    class PNGWriter {
        public:
            PNGWriter() {}

            bool Write(const std::string& file_name, const RGBFrame& rgb_frame);

        private:
            utils::File file_;
            CRCGenerator crc_generator_;
            bool WriteHeader(int width, int height);
            bool WriteFrame(const RGBFrame& rgb_frame);
            bool WriteEnd();
            void WriteData(uint8_t data);
            void WriteData(uint16_t data);
            void WriteData(uint32_t data);
            void WriteData(const std::vector<uint8_t>& data);
            void WriteAndUpdate(uint8_t data);
            void WriteAndUpdate(uint16_t data);
            void WriteAndUpdate(uint32_t data);
            void WriteAndUpdate(const std::vector<uint8_t>& data);
            void WriteCRC();
    };
} // namespace video_utils
#endif // PNG_WRITER_H
