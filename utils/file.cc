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
                return "rb";
            case FileMode::Write:
                return "wb";
            case FileMode::Append:
                return "ab";
            case FileMode::ReadUpdate:
                return "r+b";
            case FileMode::WriteUpdate:
                return "w+b";
            case FileMode::AppendUpdate:
                return "a+b";
        }
    }

    void File::Open(const string& file_name, FileMode mode) {
        file_ = unique_ptr<FILE>(fopen(file_name.data(), GetString(mode).data()));
    }

    void File::Close() {
        if (file_) {
            fclose(file_.release());
        }
    }

    bool File::error() const {
        return file_ && ferror(file_.get()) == 0;
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
