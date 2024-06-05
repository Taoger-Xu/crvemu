#ifndef BUS_H
#define BUS_H

#include <vector>
#include <cstdint>
#include "dram.hh"

class Bus {
public:
    Bus(const std::vector<uint8_t>& code);

    std::optional<uint64_t> load(uint64_t addr, uint64_t size);
    void store(uint64_t addr, uint64_t size, uint64_t value);
    
private:
    Dram dram;
};

#endif