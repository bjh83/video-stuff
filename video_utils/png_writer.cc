#include "png_writer.h"

namespace video_utils {
    using namespace std;
    using utils::File;

    namespace {
        uint32_t ConvertToNetworkEndianness(uint32_t to_convert) {
            uint8_t temp;
            temp = ((char*) &to_convert)[3];
            ((char*) &to_convert)[3] = ((char*) &to_convert)[0];
            ((char*) &to_convert)[0] = temp;
            temp = ((char*) &to_convert)[2];
            ((char*) &to_convert)[2] = ((char*) &to_convert)[1];
            ((char*) &to_convert)[1] = temp;
            return to_convert;
        }

        uint16_t ConvertToNetworkEndianness(uint16_t to_convert) {
            uint8_t temp;
            temp = ((char*) &to_convert)[1];
            ((char*) &to_convert)[1] = ((char*) &to_convert)[0];
            ((char*) &to_convert)[0] = temp;
            return to_convert;
        }

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
                    bucket_[5] = 0;
                    for (int i = 6; i < kChunkSize + 5; i++) {
                        switch(plane) {
                            case 0:
                                bucket_[i] = rgb_frame_.getR(x, y);
                                adler_.Update(bucket_[i]);
                                break;
                            case 1:
                                bucket_[i] = rgb_frame_.getG(x, y);
                                adler_.Update(bucket_[i]);
                                break;
                            case 2:
                                bucket_[i] = rgb_frame_.getB(x, y);
                                adler_.Update(bucket_[i]);
                                break;
                        }
                        plane++;
                        if (plane >= 3) {
                            plane = 0;
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
                                i++;
                                if (i >= kChunkSize + 5) {
                                    break;
                                }
                                bucket_[i] = 0;
                            }
                        }
                    }

                    if (plane + 1 >= 3 && x + 1 >= rgb_frame_.width() && y + 1 >= rgb_frame_.height()) {
                        last_chunk_ = true;
                        bucket_[0] = 1;
                    }
                    uint16_t bucket_size = ConvertToNetworkEndianness(static_cast<uint16_t>(chunk_size_ - 5));
                    bucket_[1] = ((char*)&bucket_size)[0];
                    bucket_[2] = ((char*)&bucket_size)[1];
                    bucket_size = ~bucket_size;
                    bucket_[3] = ((char*)&bucket_size)[0];
                    bucket_[4] = ((char*)&bucket_size)[1];
                    return bucket_;
                }

                uint32_t GetChecksum() { return adler_.Get(); }

                bool last_chunk() { return last_chunk_; }
                int chunk_size() { return chunk_size_; }

            private:
                vector<uint8_t> bucket_;
                const RGBFrame& rgb_frame_;
                int x = 0;
                int y = 0;
                int plane = 0;
                int chunk_size_ = kChunkSize + 5;
                bool last_chunk_ = false;
                AdlerGenerator adler_;
        };
    } // namespace

    CRCGenerator::CRCGenerator() : crc_table(256) {
        uint32_t remainder;

        for (int dividend = 0; dividend < 256; dividend++) {
            remainder = static_cast<uint32_t>(dividend);
            for (int k = 0; k < 8; k++) {
                if (remainder & 1) {
                    remainder = 0xedb88320 ^ (remainder >> 1);
                } else {
                    remainder >>= 1;
                }
            }
            crc_table[dividend] = remainder;
        }
    }

    void CRCGenerator::Update(const vector<uint8_t>& data) {
        for (int n = 0; n < static_cast<int>(data.size()); n++) {
            current_crc = crc_table[(current_crc ^ data[n]) & 0xff] ^ (current_crc >> 8);
        }
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
        return ret_val ^ current_crc;
    }

    void AdlerGenerator::Update(const vector<uint8_t>& data) {
        for (const uint8_t& byte : data) {
            a_ = (a_ + byte) % 65521;
            b_ = (b_ + a_) % 65521;
        }
    }

    void AdlerGenerator::Update(uint8_t data) {
        Update(vector<uint8_t>(1, data));
    }

    void AdlerGenerator::Update(uint32_t data) {
        for (int i = 0; i < 4; i++) {
            Update(static_cast<uint8_t>(data >> 8 * i));
        }
    }

    uint32_t AdlerGenerator::Get() {
        uint32_t ret_val = (b_ << 16) | a_;
        a_ = 1;
        b_ = 0;
        return ret_val;
    }

    void PNGWriter::WriteData(uint8_t data) {
        file_.Write(data);
    }

    void PNGWriter::WriteData(uint16_t data) {
        file_.Write(ConvertToNetworkEndianness(data));
    }

    void PNGWriter::WriteData(uint32_t data) {
        file_.Write(ConvertToNetworkEndianness(data));
    }

    void PNGWriter::WriteData(const vector<uint8_t>& data) {
        file_.Write(data);
    }

    void PNGWriter::WriteAndUpdate(uint8_t data) {
        crc_generator_.Update(data);
        file_.Write(data);
    }

    void PNGWriter::WriteAndUpdate(uint32_t data) {
        data = ConvertToNetworkEndianness(data);
        crc_generator_.Update(data);
        file_.Write(data);
    }

    void PNGWriter::WriteAndUpdate(const vector<uint8_t>& data) {
        crc_generator_.Update(data);
        file_.Write(data);
    }

    void PNGWriter::WriteCRC() {
        file_.Write(ConvertToNetworkEndianness(crc_generator_.Get()));
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

        // Write file header.
        WriteData(magic_numbers); // Do not include magic numbers in CRC.

        // Write IHDR chunk.
        WriteData(ihdr_length); // Do not include length in CRC.
        WriteAndUpdate(ihdr_message);
        WriteAndUpdate(static_cast<uint32_t>(width));
        WriteAndUpdate(static_cast<uint32_t>(height));
        WriteAndUpdate(bit_depth);
        WriteAndUpdate(color_type);
        WriteAndUpdate(compression_method);
        WriteAndUpdate(filter_method);
        WriteAndUpdate(interlace_method);
        WriteCRC();

        return file_;
    }

    bool PNGWriter::WriteFrame(const RGBFrame& rgb_frame) {
        const uint32_t byte_len = ConvertToNetworkEndianness(uint32_t(rgb_frame.height() * (1 + rgb_frame.width())
                    * 3 + 5 * Chunkifier::HowManyChunksIn(rgb_frame) + 6)); // IDAT chunk len.
        const vector<uint8_t> idat_message = {73, 68, 65, 84};
        const vector<uint8_t> deflate_header = {0x78, 0x01};

        // Write IDAT chunk.
        WriteData(byte_len); // Do not include the byte length in the CRC.
        WriteAndUpdate(idat_message);

        // Write the actual data segment.
        WriteAndUpdate(deflate_header);
        Chunkifier chunkifier(rgb_frame);
        while (!chunkifier.last_chunk()) {
            const vector<uint8_t>& chunk = chunkifier.GetNextChunk();
            crc_generator_.Update(chunk);
            file_.Write(chunk);
        }
        WriteAndUpdate(chunkifier.GetChecksum());

        WriteCRC();
        return file_;
    }

    bool PNGWriter::WriteEnd() {
        const uint32_t byte_len = ConvertToNetworkEndianness(uint32_t(0));
        const vector<uint8_t> iend_message = {73, 69, 78, 68};

        // Write IEND chunk.
        WriteData(byte_len); // Do not include chunk length in CRC.
        WriteAndUpdate(iend_message);
        // No data segment.
        WriteCRC();

        return file_;
    }

    bool PNGWriter::Write(const string& file_name, const RGBFrame& rgb_frame) {
        file_.Open(file_name, File::FileMode::Write);
        if (!WriteHeader(rgb_frame.width(), rgb_frame.height())) {
            return false;
        } else if (!WriteFrame(rgb_frame)) {
            return false;
        } else if (!WriteEnd()) {
            return false;
        }
        file_.Close();
        return true;
    }

} // namespace video_utils
