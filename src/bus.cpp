#include <stdexcept>
#include <string>

#include "bus.hh"
#include "param.hh"
#include "exception.hh"

Bus::Bus(const std::vector<uint8_t> &code) : dram(code) {}

std::optional<uint64_t> Bus::load(uint64_t addr, uint64_t size) {
    // 首先要检验地址是否合法随后调用 Dram 的方法
    if (addr >= DRAM_BASE && addr <= DRAM_END) {
        return dram.load(addr, size);
    } else {
        throw Exception(Exception::Type::LoadAccessFault, addr);
        return std::nullopt;
    }
}

void Bus::store(uint64_t addr, uint64_t size, uint64_t value) {
    if (addr >= DRAM_BASE && addr <= DRAM_END) {
        dram.store(addr, size, value);
    } else {
        throw Exception(Exception::Type::StoreAMOAccessFault, addr);
    }
}