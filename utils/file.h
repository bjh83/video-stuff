#ifndef FILE_H
#define FILE_H

#include <cstdio>
#include <cstdint>
#include <cstdlib>
#include <memory>
#include <string>
#include <vector>
#include "stream_interface.h"

namespace utils {

    class File : public StreamInterface {
        public:
            enum class FileMode {
                Read,
                Write,
                Append,
                ReadUpdate,
                WriteUpdate,
                AppendUpdate
            };

            File(const std::string& file_name, FileMode mode);
            ~File();

            void Open(const std::string& file_name, FileMode mode);
            void Close();

            static std::string GetString(FileMode mode);

            virtual uint8_t Read_uint8();
            virtual uint16_t Read_uint16();
            virtual uint32_t Read_uint32();
            virtual std::vector<uint8_t> Read(size_t size);

            virtual void Write(uint8_t value);
            virtual void Write(uint16_t value);
            virtual void Write(uint32_t value);
            virtual void Write(const std::vector<uint8_t>& value);

        private:
            std::unique_ptr<FILE> file_;
    };
} // namespace utils
#endif // FILE_H
