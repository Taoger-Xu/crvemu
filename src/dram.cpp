#include <iostream>
#include <stdexcept>

#include "dram.hh"
#include "exception.hh"
#include "param.hh"

Dram::Dram(const std::vector<uint8_t> &code) {

    // 使用0初始化DRAM。
    dram.resize(DRAM_SIZE, 0);

    // std::copy是标准库算法，用于复制一个范围内的元素到另一个范围。
    // code.begin()和code.end()分别指向传入vector的开始和结束，指定了要复制的数据范围。
    // dram.begin()指定了目标范围的开始位置。
    std::copy(code.begin(), code.end(), dram.begin());
}

// 输入参数为 addr 表示内存地址，size 表示需要读取的长度
std::optional<uint64_t> Dram::load(uint64_t addr, uint64_t size) {

    try {
        if (size != 8 && size != 16 && size != 32 && size != 64) {
            throw Exception(Exception::Type::LoadAccessFault, addr);
        }

        uint64_t nbytes = size / 8;
        std::size_t index = (addr - DRAM_BASE);
        if (index + nbytes > dram.size()) {
            throw std::out_of_range("Address out of range");
        }

        uint64_t value = 0;

        // 小段序，index + i，i越小，对应的有效位越低
        for (uint64_t i = 0; i < nbytes; i++) {
            value |= static_cast<uint64_t>(dram[index + i]) << (i * 8);
        }
        return value;

    } catch (const Exception &e) {
        std::cerr << "Exception Dram::load: " << e << std::endl;
        return std::nullopt;
    }
}

void Dram::store(uint64_t addr, uint64_t size, uint64_t value) {

    try {
        if (size != 8 && size != 16 && size != 32 && size != 64) {
            throw Exception(Exception::Type::StoreAMOAccessFault, addr);
        }
        uint64_t nbytes = size / 8;
        std::size_t index = (addr - DRAM_BASE);
        if (index + nbytes > dram.size()) {
            throw std::out_of_range("Address out of range");
        }

        for (uint64_t i = 0; i < nbytes; i++) {
            dram[index + i] = (value >> (i * 8)) & 0xFF;
        }

    } catch (const Exception &e) {
        std::cerr << "Exception Dram::store: " << e << std::endl;
    }
}