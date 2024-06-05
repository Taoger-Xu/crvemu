#include <array>
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <vector>

// 定义DRAM大小为 128MB
const uint64_t DRAM_SIZE = 1024 * 1024 * 128;

// 处理器定义
class CPU {
public:
    // RISC-有32个寄存器，每个寄存器64位
    std::array<uint64_t, 32> regs;

    // PC寄存器
    uint64_t pc;

    // 内存，是一个逻辑数组
    std::vector<uint8_t> dram;

    // RISC-V 寄存器名称
    const std::array<std::string, 32> RVABI = {
        "zero", "ra", "sp", "gp", "tp",  "t0",  "t1", "t2", "s0", "s1", "a0",
        "a1",   "a2", "a3", "a4", "a5",  "a6",  "a7", "s2", "s3", "s4", "s5",
        "s6",   "s7", "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6",
    };

public:
    // 用elf读取的二进制中的code，初始化内存
    CPU(const std::vector<uint8_t> &code) : pc{0}, dram(code) {
        regs.fill(0); // 所有寄存器初始化为0
        regs[2] = DRAM_SIZE -
                  1; // 栈指针 (SP) 需要指向栈顶（内存的最高地址，x2即sp，栈指针
    }

    // 用pc的值去内存取出指令，riscv是32位定长指令
    uint32_t fetch() {
        // 访问 dram 时需要使用 size_t 类型的索引来确保索引的类型和数组匹配
        size_t index = static_cast<size_t>(pc);

        // 采用小端字节序，即低地址对应指令的低位数

        uint32_t inst = static_cast<uint32_t>(dram[index]) |
                        static_cast<uint32_t>(dram[index + 1] << 8) |
                        static_cast<uint32_t>(dram[index + 2] << 16) |
                        static_cast<uint32_t>(dram[index + 3] << 24);
        return inst;
    }

    void execute(uint32_t inst) {
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

        // execute stage, 这一步就是抄手册
        switch (opcode) {
        case 0x13: { // addi
            int64_t imm = static_cast<int32_t>(inst & 0xfff00000) >> 20;
            regs[rd] = regs[rs1] + imm;
            break;
        }
        case 0x33: { // add
            regs[rd] = regs[rs1] + regs[rs2];
            break;
        }
        default:
            std::cerr << "Invalid opcode: " << std::hex << opcode << std::endl;
            break;
        }
    }
    void dump_registers() {
    const std::string GREEN = "\033[01;32m";  // 绿色开始
    const std::string RED = "\033[01;31m";    // 红色开始
    const std::string RESET = "\033[0m";      // 颜色重置
    const int ALIGN_WIDTH = 28;               // 用于对齐'='的位置

    std::cout << std::setw(90) << std::setfill('-') << "" << std::endl; // 打印分隔线
    std::cout << std::setfill(' '); // 重置填充字符

    for (size_t i = 0; i < 32; i += 2) {
        std::string reg0 = "x" + std::to_string(i) + RED + "(" + RVABI[i] + ")" + RESET;
        std::string reg1 = "x" + std::to_string(i + 1) + RED + "(" + RVABI[i + 1] + ")" + RESET;

        uint64_t val0 = regs[i];
        uint64_t val1 = regs[i + 1];

        std::cout << std::right << std::setw(10) << reg0;

        std::cout << std::setfill(' '); // 重置填充字符
        std::cout << std::right << std::setw(3) << " = "; // 等号前3个字符（包括空格）
        std::cout << "0x" << std::hex << std::setw(16) << std::setfill('0') << val0
                  << GREEN << "(" << std::dec << val0 << ")" << RESET;

        std::cout << std::setfill(' '); // 重置填充字符

        std::cout << std::setw(10) << " "; // 对齐等号，减去等号前3个字符的宽度

        
        std::cout << std::right << std::setw(10) << reg1;

        std::cout << std::setfill(' '); // 重置填充字符
        std::cout << std::right << std::setw(3) << " = "; // 等号前3个字符（包括空格）
        std::cout << "0x" << std::hex << std::setw(16) << std::setfill('0') << val1
                  << GREEN << "(" << std::dec << val1 << ")" << RESET
                  << std::endl;

    }

    std::cout << std::setw(90) << std::setfill('-') << "" << std::endl; // 结束分隔线
}


};

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cout << "Usage:\n"
                  << "- ./program_name <filename>\n";
        return 0;
    }

    // 默认情况下，文件是以文本模式打开的。在文本模式下，某些字符可能会被转换（例如，在
    // Windows 系统上，\n 可能会被转换为 \r\n）。 使用 std::ios::binary
    // 标志可以防止这些转换，确保文件内容按原样读取
    std::ifstream file(argv[1], std::ios::binary);

    // 打开文件失败
    if (!file) {
        std::cerr << "Cannot open file: " << argv[1] << std::endl;
        return 1;
    }

    // 用elf文件中的内存初始化code
    std::vector<uint8_t> code(std::istreambuf_iterator<char>(file), {});
    CPU cpu(code);

    // 执行完内存所有指令，其中sizeof(cpu.dram[0])是每条指令的字长
    while (cpu.pc < cpu.dram.size() * sizeof(cpu.dram[0])) {
        uint32_t inst = cpu.fetch();
        cpu.execute(inst);
        cpu.pc += 4;
    }

    // 打印寄存器状态
    cpu.dump_registers();
    return 0;
}