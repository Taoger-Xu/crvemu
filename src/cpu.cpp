#include <fstream>
#include <iomanip> // 用于格式化输出
#include <iostream>
#include <optional>

#include "cpu.hh"
#include "exception.hh"

std::optional<uint64_t> Cpu::load(uint64_t addr, uint64_t size) {
    try {
        return bus.load(addr, size);
    } catch (const Exception &e) {
        std::cerr << "Exception load: " << e << std::endl;
        return std::nullopt;
    }
}

void Cpu::store(uint64_t addr, uint64_t size, uint64_t value) {
    try {
        bus.store(addr, size, value);
    } catch (const Exception &e) {
        std::cerr << "Exception store: " << e << std::endl;
    }
}

std::optional<uint32_t> Cpu::fetch() {
    try {
        auto inst = bus.load(pc, 32);
        if(inst.has_value()) {
            return inst.value();
        }
        throw Exception(Exception::Type::InstructionAccessFault, pc);
    } catch (const Exception &e) {
        std::cerr << "Exception fetch: " << e << std::endl;
        return std::nullopt;
    }
}

std::optional<uint64_t> Cpu::execute(uint32_t inst) {
    try {
        // decode, 按照RV手册解码指令
        uint32_t opcode = inst & 0x7f;
        uint32_t rd = (inst >> 7) & 0x1f;
        uint32_t rs1 = (inst >> 15) & 0x1f;
        uint32_t rs2 = (inst >> 20) & 0x1f;
        uint32_t funct3 = (inst >> 12) & 0x7;
        uint32_t funct7 = (inst >> 25) & 0x7f;

        // 按照手册解释指令语义，改变状态机
        // x0是zero寄存器，始终为0
        regs[0] = 0;

        // debug
        std::cout << "Executing instruction: 0x" << std::hex << inst << std::dec
                  << std::endl;

        switch (opcode) {
        case 0x13: { // addi
            int64_t imm = static_cast<int32_t>(inst & 0xfff00000) >> 20;
            regs[rd] = regs[rs1] + imm;

            // debug
            std::cout << "ADDI: x" << rd << " = x" << rs1 << " + " << imm
                      << std::endl;
            return update_pc();
        }
        case 0x33: { // add
            regs[rd] = regs[rs1] + regs[rs2];
            // debug
            std::cout << "ADD: x" << rd << " = x" << rs1 << " + x" << rs2
                      << std::endl;
            return update_pc();
        }
        default:
            // 抛出自定义异常
            throw Exception(Exception::Type::IllegalInstruction, opcode);
        }

    } catch (const Exception &e) {
        std::cerr << "Exception execute : " << e << std::endl;
        return std::nullopt; // 使用 std::optional 表示异常
    }
}

// 打印寄存器组
void Cpu::dump_registers() const {
    const std::string GREEN = "\033[01;32m"; // 绿色开始
    const std::string RED = "\033[01;31m";   // 红色开始
    const std::string RESET = "\033[0m";     // 颜色重置
    const int ALIGN_WIDTH = 28;              // 用于对齐'='的位置

    std::cout << std::setw(90) << std::setfill('-') << ""
              << std::endl;         // 打印分隔线
    std::cout << std::setfill(' '); // 重置填充字符

    for (size_t i = 0; i < 32; i += 2) {
        std::string reg0 =
            "x" + std::to_string(i) + RED + "(" + RVABI[i] + ")" + RESET;
        std::string reg1 = "x" + std::to_string(i + 1) + RED + "(" +
                           RVABI[i + 1] + ")" + RESET;

        uint64_t val0 = regs[i];
        uint64_t val1 = regs[i + 1];

        std::cout << std::right << std::setw(10) << reg0;

        std::cout << std::setfill(' '); // 重置填充字符
        std::cout << std::right << std::setw(3)
                  << " = "; // 等号前3个字符（包括空格）
        std::cout << "0x" << std::hex << std::setw(16) << std::setfill('0')
                  << val0 << GREEN << "(" << std::dec << val0 << ")" << RESET;

        std::cout << std::setfill(' '); // 重置填充字符

        std::cout << std::setw(10) << " "; // 对齐等号，减去等号前3个字符的宽度

        std::cout << std::right << std::setw(10) << reg1;

        std::cout << std::setfill(' '); // 重置填充字符
        std::cout << std::right << std::setw(3)
                  << " = "; // 等号前3个字符（包括空格）
        std::cout << "0x" << std::hex << std::setw(16) << std::setfill('0')
                  << val1 << GREEN << "(" << std::dec << val1 << ")" << RESET
                  << std::endl;
    }

    std::cout << std::setw(90) << std::setfill('-') << ""
              << std::endl; // 结束分隔线
}

// 打印pc
void Cpu::dump_pc() const {
    const std::string GREEN = "\033[01;32m"; // 绿色开始
    const std::string RESET = "\033[0m";     // 颜色重置
    std::cout << std::setw(80) << std::setfill('-') << "" << std::endl;
    std::cout << "PC register" << std::endl;
    std::cout << GREEN << "PC = " << RESET << "0x" << std::hex << pc << std::dec
              << std::endl;
    std::cout << std::setw(80) << std::setfill('-') << "" << std::endl;
}