cmake_minimum_required(VERSION 3.22.1)
project(crvemu)

set(CMAKE_CXX_STANDARD 23)

# 指定了一个名为 crvemu 的可执行文件，并且该可执行文件的源文件是 main.cpp
add_executable(crvemu main.cpp)

# 启用测试支持，并添加 googletest 子目录，
enable_testing()
add_subdirectory("lib/googletest")

# 名为 g_test 的可执行文件，并且该可执行文件的源文件是 g_test.cc。g_test.cc 会包含测试代码
add_executable(
        g_test
        g_test.cc
)

# 将 GTest::gtest_main 库链接到 g_test 可执行文件中。GTest::gtest_main 是 Google Test 提供的一个目标，包含了主测试入口
target_link_libraries(
        g_test
        GTest::gtest_main
)

# 首先包含了 Google Test 的 CMake 模块，然后使用 gtest_discover_tests 自动发现和注册所有测试。
# 这样，在运行 ctest 时，所有在 g_test 中定义的测试都会被自动执行
include(GoogleTest)
gtest_discover_tests(g_test)