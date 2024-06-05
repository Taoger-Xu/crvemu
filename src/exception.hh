#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <cstdint>
#include <iostream>

// 自定义异常处理
class Exception {
public:
    enum class Type {
        InstructionAddrMisaligned,
        InstructionAccessFault,
        IllegalInstruction,
        Breakpoint,
        LoadAccessMisaligned,
        LoadAccessFault,
        StoreAMOAddrMisaligned,
        StoreAMOAccessFault,
        EnvironmentCallFromUMode,
        EnvironmentCallFromSMode,
        EnvironmentCallFromMMode,
        InstructionPageFault,
        LoadPageFault,
        StoreAMOPageFault
    };

private:
    Type type;
    uint64_t value;
public:
    Exception(Type type, uint64_t value);

    // 重载异常类型的输出信息
    friend std::ostream& operator<<(std::ostream& os, const Exception& ex);

    // 得到异常值
    uint64_t getValue() const;

    uint64_t getCode() const;

    bool isFatal() const;

};

#endif