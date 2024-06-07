#include <fstream>
#include <vector>

#include "src/cpu.hh"
#include "gtest/gtest.h"

void generate_rv_assembly(const std::string &c_src) {
    std::string command_line =
        " riscv64-unknown-elf-gcc -S" + c_src + "-o output.s";
    int result = std::system(command_line.c_str());

    if (result != 0) {
        throw std::runtime_error("Failed to generate RV assembly. Command: " +
                                 command_line);
    }
}

void generate_rv_obj(const std::string &assembly) {
    // 使用C++的字符串处理能力来获取不含扩展名的文件名
    size_t dotPos = assembly.find_last_of(".");
    std::string baseName =
        (dotPos == std::string::npos) ? assembly : assembly.substr(0, dotPos);

    std::string command_line = " riscv64-unknown-elf-gcc -Wl,-Ttext=0x0 "
                               "-nostdlib -march=rv64g -mabi=lp64 -o " +
                               baseName + " " + assembly;

    // 执行命令
    int result = std::system(command_line.c_str());

    // 检查命令执行结果
    if (result != 0) {
        std::cerr << "Failed to generate RV object from assembly: " << assembly
                  << std::endl;
    }
}

void generate_rv_binary(const std::string &obj) {
    std::string command_line =
        " riscv64-unknown-elf-objcopy -O binary " + obj + " " + obj + ".bin";

    // 执行命令
    int result = std::system(command_line.c_str());

    // 检查命令执行结果
    if (result != 0) {
        std::cerr << "Failed to generate RV binary from object: " << obj
                  << std::endl;
    }
}

// 生成测试用的Cpu实例，code是测试指令，test_name是测试用例名称，n_clock是周期数
// 比如 code = "addi x31, x0, 0" test_name = "test-addi"
Cpu rv_helper(const std::string &code, const std::string &test_name,
              size_t n_clock) {
    // 首先根据test_name创建对应的汇编文件
    std::string filename = test_name + ".s";

    // 将指令写入汇编文件，即创建
    std::ofstream file(filename);

    if (!file.is_open()) {
        throw std::runtime_error("Failed to create assembly file.");
    }

    file << code;
    file.close();

    // 生成目标文件和二进制文件
    generate_rv_obj(filename);
    generate_rv_binary(test_name);

    // 读取二进制文件
    std::string bin_name = test_name + ".bin";
    std::ifstream file_bin(bin_name, std::ios::binary);

    if (!file_bin.is_open()) {
        throw std::runtime_error("Failed to open binary file.");
    }

    std::vector<uint8_t> bin_code(std::istreambuf_iterator<char>(file_bin), {});

    Cpu cpu(bin_code);

    for (size_t i = 0; i < n_clock; i++) {
        try {
            auto inst = cpu.fetch();
            auto next_pc = cpu.execute(inst.value());

            if (next_pc.has_value()) {
                cpu.pc = next_pc.value();
            } else {
                break;
            }
        } catch (const std::exception &e) {
            std::cerr << "CPU execution error: " << e.what() << std::endl;
            break;
        }
    }

    return cpu;
}

// 消除警告： warning: cannot find entry symbol _start; defaulting to
// 0000000000000000
const std::string start = ".global _start \n _start: \n";

// Test addi instruction
TEST(RVTests, TestAddi) {
    std::string code = start + "addi x31, x0, 42 \n";
    Cpu cpu = rv_helper(code, "test_addi", 1);
    EXPECT_EQ(cpu.regs[31], 42)
        << "Error: x31 should be 42 after ADDI instruction";
}

// Test add instruction
TEST(RVTests, TestAdd) {
    std::string code = start + "addi x2, x0, 10 \n"
                               "addi x3, x0, 20 \n"
                               "add  x1, x2, x3 \n";
    ;
    Cpu cpu = rv_helper(code, "test_add", 3);
    EXPECT_EQ(cpu.regs[1], 30)
        << "Error: x1 should be the result of ADD instruction";
}