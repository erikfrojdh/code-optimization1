#pragma once

#include <cstdio>

template <typename T>
class Reader {
  private:
    mutable std::mutex m_;
    size_t count_;
    int num_reads_ = 0;
    int value_ = 0;
    FILE *pFile_;

  public:
    Reader(size_t count) : count_(count) {}
    FILE* open(const std::string& fname){
        pFile_ = std::fopen(fname.c_str(), "rb");
        return pFile_;
    }

    size_t read(T *dest) {
        std::unique_lock<std::mutex> lock(m_);
        num_reads_++;
        return std::fread(dest, sizeof(T), count_, pFile_);
        
    }

    int num_reads() const{
        std::unique_lock<std::mutex> lock(m_);
        return num_reads_;
    }
};


// std::fread(hits.data(), sizeof(Hit), buffer_size, pFile);