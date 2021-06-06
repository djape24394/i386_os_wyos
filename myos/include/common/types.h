#ifndef MYOS_COMMON_TYPES_H
#define MYOS_COMMON_TYPES_H

namespace myos::common
{
    typedef char int8_t;
    typedef unsigned char uint8_t;

    typedef short int16_t;
    typedef unsigned short uint16_t;

    typedef int int32_t;
    typedef unsigned int uint32_t;

    typedef long long int int64_t;
    typedef unsigned long long int uint64_t;
    typedef uint32_t size_t; // on 32 bit system, addresses are 32 bit
} // namespace myos::common

#endif // MYOS_COMMON_TYPES_H
