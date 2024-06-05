#ifndef CPU_H
#define CPU_H

#include "bus.hh"
#include "param.hh"
#include <array>
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <optional>
#include <vector>

// 处理器定义
class Cpu {
public:
    // RISC-有32个寄存器，每个寄存器64位
    // 当用空的 {} 进行列表初始化时，如果元素类型是基本类型（如
    // int、float、uint64_t 等），它们将被初始化为零值
    std::array<uint64_t, 32> regs{};

    // PC寄存器
    uint64_t pc;

    // CPU通过总线和内存交互
    Bus bus;

    Cpu(const std::vector<uint8_t> &code)
        : pc{DRAM_BASE}, bus{code},
          RVABI{"zero", "ra", "sp",  "gp",  "tp", "t0", "t1", "t2",
                "s0",   "s1", "a0",  "a1",  "a2", "a3", "a4", "a5",
                "a6",   "a7", "s2",  "s3",  "s4", "s5", "s6", "s7",
                "s8",   "s9", "s10", "s11", "t3", "t4", "t5", "t6"} {
        regs.fill(0); // 所有寄存器初始化为0
        regs[2] = DRAM_SIZE -
                  1; // 栈指针 (SP) 需要指向栈顶（内存的最高地址，x2即sp，栈指针
    }

    std::optional<uint64_t> load(uint64_t addr, uint64_t size);

    void store(uint64_t addr, uint64_t size, uint64_t value);

    // 取出32位字长的指令
    std::optional<uint32_t> fetch();

    // 返回下一条指令的地址
    [[nodiscard]] inline uint64_t update_pc() const {
        return pc + 4;
    }

    // 打印寄存器组
    void dump_registers() const;

    // 打印pc
    void dump_pc() const;

    // 执行当前指令，并且返回下一条指令的地址
    std::optional<uint64_t>  execute(uint32_t inst);

private:
    // RISC-V 寄存器名称
    const std::array<std::string, 32> RVABI;
};

#endif