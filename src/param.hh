#ifndef PARAM_H
#define PARAM_H

#include <cstddef> // 引入定义 std::size_t 的头文件

// 定义DRAM的基地址
constexpr std::size_t DRAM_BASE = 0;

// 定义DRAM的大小为128MB
constexpr std::size_t DRAM_SIZE = 1024 * 1024 * 128;

// 定义DRAM的结束地址
constexpr std::size_t DRAM_END = DRAM_SIZE + DRAM_BASE - 1;

#endif