#ifndef MYOS_COMMON_PRINT_TO_CONSOLE_H
#define MYOS_COMMON_PRINT_TO_CONSOLE_H

#include <common/types.h>

namespace myos::common
{
    void printf(const char *str);

    void printfHex(common::uint8_t key);

    void printfHex16(common::uint16_t key);

    void printfHex32(common::uint32_t key);
} // namespace myos::common

#endif // MYOS_COMMON_PRINT_TO_CONSOLE_H
