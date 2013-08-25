#include "file.h"

namespace utils {
    using namespace std;

    File::File(const string& file_name, FileMode mode) {
        Open(file_name, mode);
    }

    File::~File() {
        Close();
    }

    string File::GetString(FileMode mode) {
        switch (mode) {
            case FileMode::Read:
                return "r";
            case FileMode::Write:
                return "w";
            case FileMode::Append:
                return "a";
            case FileMode::ReadUpdate:
                return "r+";
            case FileMode::WriteUpdate:
                return "w+";
            case FileMode::AppendUpdate:
                return "a+";
        }
    }

    void File::Open(const string& file_name, FileMode mode) {
        file_ = unique_ptr<FILE>(fopen(file_name.data(), GetString(mode).data()));
    }

    void File::Close() {
        fclose(file_.get());
    }

    uint8_t File::Read_uint8() {
        uint8_t ret_val;
        fread(&ret_val, sizeof(ret_val), 1, file_.get());
        return ret_val;
    }

    uint16_t File::Read_uint16() {
        uint16_t ret_val;
        fread(&ret_val, sizeof(ret_val), 1, file_.get());
        return ret_val;
    }

    uint32_t File::Read_uint32() {
        uint32_t ret_val;
        fread(&ret_val, sizeof(ret_val), 1, file_.get());
        return ret_val;
    }

    vector<uint8_t> File::Read(size_t size) {
        uint8_t* data = nullptr;
        vector<uint8_t> ret_val(size);
        fread(data, sizeof(uint8_t), size, file_.get());
        for (size_t i = 0; i < size; i++) {
            ret_val[i] = data[i];
        }
        delete[] data;
        return ret_val;
    }

    void File::Write(uint8_t value) {
        fwrite(&value, sizeof(value), 1, file_.get());
    }

    void File::Write(uint16_t value) {
        fwrite(&value, sizeof(value), 1, file_.get());
    }

    void File::Write(uint32_t value) {
        fwrite(&value, sizeof(value), 1, file_.get());
    }

    void File::Write(const vector<uint8_t>& value) {
        fwrite(value.data(), sizeof(uint8_t), value.size(), file_.get());
    }

} // namespace utils
