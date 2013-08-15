#include "png_writer.h"

namespace video_utils {
    using namespace std;

    namespace {
        class Chunkifier {
            public:
                Chunkifier(const RGBFrame& rgb_frame) : bucket_(kChunkSize), rgb_frame_(rgb_frame) {}
                static const int kChunkSize = 65535;

                static int HowManyChunksIn(const RGBFrame& rgb_frame) {
                    const int byte_len = rgb_frame.width() * rgb_frame.height() * 3;
                    int chunks = byte_len / kChunkSize;
                    if (byte_len % kChunkSize != 0) {
                        chunks++;
                    }
                    return chunks;
                }

                const vector<uint8_t>& GetNextChunk() {
                    bucket_[0] = 0;
                    for (int i = 1; i < kChunkSize + 1; i++) {
                        plane++;
                        if (plane >= 3) {
                            x++;
                            if (x >= rgb_frame_.width()) {
                                x = 0;
                                y++;
                                if (y >= rgb_frame_.height()) {
                                    last_chunk_ = true;
                                    chunk_size_ = i;
                                    bucket_.resize(chunk_size_);
                                    bucket_[0] = 1;
                                    break;
                                }
                            }
                        }
                        switch(plane) {
                            case 0:
                                bucket_[i] = rgb_frame_.getR(x, y);
                                break;
                            case 1:
                                bucket_[i] = rgb_frame_.getG(x, y);
                                break;
                            case 2:
                                bucket_[i] = rgb_frame_.getB(x, y);
                                break;
                        }
                    }

                    if (plane + 1 >= 3 && x + 1 >= rgb_frame_.width() && y + 1 >= rgb_frame_.height()) {
                        last_chunk_ = true;
                        bucket_[0] = 1;
                    }
                    return bucket_;
                }

                bool last_chunk() { return last_chunk_; }
                int chunk_size() { return chunk_size_; }

            private:
                vector<uint8_t> bucket_;
                const RGBFrame& rgb_frame_;
                int x = 0;
                int y = 0;
                int plane = 0;
                int chunk_size_ = kChunkSize;
                bool last_chunk_ = false;
        };
    } // namespace

    CRCGenerator::CRCGenerator() : crc_table(256) {
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

    void CRCGenerator::Update(const vector<uint8_t>& data) {
        for (int n = 0; n < static_cast<int>(data.size()); n++) {
            const int i = ((current_crc >> 24) ^ data[n]) & 0xff;
            current_crc = crc_table[i] ^ (current_crc << 8);
        }
        current_crc = ~current_crc;
    }

    void CRCGenerator::Update(uint8_t data) {
        Update(vector<uint8_t>(1, data));
    }

    void CRCGenerator::Update(uint32_t data) {
        for (int i = 0; i < 4; i++) {
            Update(static_cast<uint8_t>(data >> 8 * i));
        }
    }

    uint32_t CRCGenerator::Get() {
        uint32_t ret_val = current_crc;
        current_crc = 0xffffffff;
        return ret_val;
    }

    bool PNGWriter::WriteHeader(int width, int height) {
        const vector<uint8_t> magic_numbers = {137, 80, 78, 71, 13, 10, 26, 10};
        const uint32_t ihdr_length = 13;
        const vector<uint8_t> ihdr_message = {73, 72, 68, 82};
        const uint8_t bit_depth = 8;
        const uint8_t color_type = 2;
        const uint8_t compression_method = 0;
        const uint8_t filter_method = 0;
        const uint8_t interlace_method = 0;
        file_.write((char*)magic_numbers.data(), magic_numbers.size());
        file_ << ihdr_length;
        // Do not include length in CRC
        file_.write((char*)ihdr_message.data(), ihdr_message.size());
        crc_generator_.Update(ihdr_message);
        file_ << width;
        crc_generator_.Update(static_cast<uint32_t>(width));
        file_ << height;
        crc_generator_.Update(static_cast<uint32_t>(height));
        file_ << bit_depth;
        crc_generator_.Update(bit_depth);
        file_ << color_type;
        crc_generator_.Update(color_type);
        file_ << compression_method;
        crc_generator_.Update(compression_method);
        file_ << filter_method;
        crc_generator_.Update(filter_method);
        file_ << interlace_method;
        crc_generator_.Update(interlace_method);
        file_ << crc_generator_.Get();
        return file_;
    }

    bool PNGWriter::WriteFrame(const RGBFrame& rgb_frame) {
        const uint32_t byte_len = rgb_frame.height() * rgb_frame.width() * 3 + Chunkifier::HowManyChunksIn(rgb_frame); // IDAT chunk len
        const vector<uint8_t> idat_message = {73, 68, 65, 84};
        file_ << byte_len;
        file_.write((char*)idat_message.data(), idat_message.size());
        crc_generator_.Update(idat_message);

        Chunkifier chunkifier(rgb_frame);
        while (!chunkifier.last_chunk()) {
            const vector<uint8_t>& chunk = chunkifier.GetNextChunk();
            file_.write((char*)chunk.data(), chunkifier.chunk_size());
            crc_generator_.Update(chunk);
        }
        file_ << crc_generator_.Get();
        return file_;
    }

    bool PNGWriter::WriteEnd() {
        const uint32_t byte_len = 0;
        const vector<uint8_t> iend_message = {73, 69, 78, 68};
        file_ << byte_len;
        file_.write((char*)iend_message.data(), iend_message.size());
        crc_generator_.Update(iend_message);
        file_ << crc_generator_.Get();
        return file_;
    }

    bool PNGWriter::Write(const string& file_name, const RGBFrame& rgb_frame) {
        file_ = fstream(file_name);
        if (!WriteHeader(rgb_frame.width(), rgb_frame.height())) {
            return false;
        } else if (!WriteFrame(rgb_frame)) {
            return false;
        } else if (!WriteEnd()) {
            return false;
        }
        file_.close();
        return true;
    }

} // namespace video_utils
