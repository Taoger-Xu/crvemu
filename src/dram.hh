#ifndef DRAM_H
#define DRAM_H

#include <cstdint>
#include <optional>
#include <vector>

// 内存（DRAM）只有两个功能：store，load。保存和读取的有效位数是 8，16，32，64
class Dram {
public:
    Dram();

    Dram(const std::vector<uint8_t> &code);

    std::optional<uint64_t> load(uint64_t addr, uint64_t size);

    void store(uint64_t addr, uint64_t size, uint64_t value);

private:
    std::vector<uint8_t> dram;
};

#endif