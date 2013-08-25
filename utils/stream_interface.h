#ifndef STREAM_INTERFACE_H
#define STREAM_INTERFACE_H

#include <cstdint>
#include <cstdlib>
#include <vector>

namespace utils {

    class StreamInterface {
        public:
            virtual uint8_t Read_uint8() = 0;
            virtual uint16_t Read_uint16() = 0;
            virtual uint32_t Read_uint32() = 0;
            virtual std::vector<uint8_t> Read(size_t size) = 0;

            virtual void Write(uint8_t value) = 0;
            virtual void Write(uint16_t value) = 0;
            virtual void Write(uint32_t value) = 0;
            virtual void Write(const std::vector<uint8_t>& value) = 0;
    };
} // namespace utils
#endif // STREAM_INTERFACE_H
