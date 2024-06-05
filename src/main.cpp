#include "cpu.hh"
#include "exception.hh"
#include <cstdint>
#include <fstream>
#include <iostream>
#include <vector>

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
    Cpu cpu(code);

    // 执行完内存所有指令，其中sizeof(cpu.dram[0])是每条指令的字长
    try {
        /* code */
        while (true) {
            auto inst = cpu.fetch();
            if (inst.has_value()) {
                auto next_pc = cpu.execute(inst.value());
                if (next_pc.has_value()) {
                    cpu.pc = next_pc.value();
                } else {
                    break;
                }
            } else {
                break;
            }
        }

    } catch (const Exception &e) {
        std::cerr << "Exception main: " << e << std::endl;
    }

    // 打印寄存器和PC状态
    cpu.dump_registers();
    cpu.dump_pc();
    return 0;
}
