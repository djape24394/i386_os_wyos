#include "KeyboardDriver.h"

void printf(char *);

KeyboardDriver::KeyboardDriver(InterruptManager* interrupt_manager)
: InterruptHandler(0x21, interrupt_manager),
  data_port(0x60),
  command_port(0x64)
{
    // when you start operating system, this will just desc
    while (command_port.read() & 0x1)
    {
        data_port.read();
    }
    command_port.write(0xAE); // tells something to keyboard
    command_port.write(0x20); // get current state
    uint8_t status =(data_port.read() | 1) & (~0x10);
    command_port.write(0x60); // set state
    data_port.write(status);
    data_port.write(0xF4); // activate keyboard
}

uint32_t KeyboardDriver::handleInterrupt(uint32_t esp)
{
    uint8_t key = data_port.read();
    // TODO: add all qwerty letters with shift to the switch
    // For each click we get two interrupts, one for press and one for release. 
    // key >= 0x80 is for the release and we ignore it
    if(key < 0x80)
    {
        switch (key)
        {
        // ignoring some codes at the beginning, and the codes from virtual machine stuff
        case 0xFA:
            break;
        case 0x45: case 0xC5:
            break;        
        case 0x1E:
            printf("a");
            break;
        default:
            char* foo = "KEYBOARD 0x00 ";
            char* hex = "0123456789ABCDEF";
            foo[11] = hex[(key >> 4) & 0x0F];
            foo[12] = hex[key & 0x0F];
            printf(foo);
            break;
        }
    }

    return esp;
}

KeyboardDriver::~KeyboardDriver(){}
