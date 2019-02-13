#pragma once

#include <cstdio>
#include <mutex>
#include <stdexcept>

template <typename T, size_t count>
class ThreadSafeReader {
  private:
    mutable std::mutex m_;
    int num_reads_ = 0;
    FILE *pFile_;

  public:
    ThreadSafeReader(const std::string &fname) : pFile_(std::fopen(fname.c_str(), "rb")) {
        if (!pFile_) {
            throw std::runtime_error("File not found\n");
        }
    }

    void close() {
        if (pFile_) {
            std::fclose(pFile_);
            pFile_ = nullptr;
        }
    }

    size_t read(T *dest) {
        std::unique_lock<std::mutex> lock(m_);
        num_reads_++;
        return std::fread(dest, sizeof(T), count, pFile_);
    }

    int num_reads() const {
        std::unique_lock<std::mutex> lock(m_);
        return num_reads_;
    }
};
