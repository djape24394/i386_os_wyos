#include "Port.h"

Port::Port(uint16_t port_number) : port_number(port_number) {}

Port8bit::Port8bit(uint16_t port_number) : Port(port_number) {}
void Port8bit::write(uint8_t data)
{
    // https://www.codeproject.com/Articles/15971/Using-Inline-Assembly-in-C-C
    // https://www.ibiblio.org/gferg/ldp/GCC-Inline-Assembly-HOWTO.html
    asm volatile("outb %0, %1"
                 :
                 : "a"(data), "Nd"(port_number));
}

uint8_t Port8bit::read()
{
    uint8_t result;
    asm volatile("inb %1, %0"
                 : "=a"(result)
                 : "Nd"(port_number));
    return result;
}

Port8bitSlow::Port8bitSlow(uint16_t port_number) : Port8bit(port_number){}

void Port8bitSlow::write(uint8_t data)
{
    asm volatile("outb %0, %1\njmp 1f\n1: jmp 1f\n1:"
                 :
                 : "a"(data), "Nd"(port_number));
}

Port16bit::Port16bit(uint16_t port_number) : Port(port_number) {}
void Port16bit::write(uint16_t data)
{
    asm volatile("outw %0, %1"
                 :
                 : "a"(data), "Nd"(port_number));
}

uint16_t Port16bit::read()
{
    uint16_t result;
    asm volatile("inw %1, %0"
                 : "=a"(result)
                 : "Nd"(port_number));
    return result;
}

Port32bit::Port32bit(uint16_t port_number) : Port(port_number) {}
void Port32bit::write(uint32_t data)
{
    asm volatile("outl %0, %1"
                 :
                 : "a"(data), "Nd"(port_number));
}

uint32_t Port32bit::read()
{
    uint32_t result;
    asm volatile("inl %1, %0"
                 : "=a"(result)
                 : "Nd"(port_number));
    return result;
}
