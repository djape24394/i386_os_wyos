#include <system_calls.h>

using namespace myos;
using namespace myos::common;
using namespace myos::hardware_communication;

void printf(char *);

SystemCallHandler::SystemCallHandler(InterruptManager *interrupt_manager, uint8_t interrupt_number)
    : InterruptHandler(interrupt_number + 0x20U, interrupt_manager)
{
}

SystemCallHandler::~SystemCallHandler()
{
    // TODO: fix destructors
}

uint32_t SystemCallHandler::handleInterrupt(uint32_t esp)
{
    CPUstate *cpu_state_pointer = (CPUstate *) esp;

    switch (cpu_state_pointer->eax)
    {
    case 4:
        printf((char*)(cpu_state_pointer->ebx));
        break;
    
    default:
        break;
    }

    return esp;
}
