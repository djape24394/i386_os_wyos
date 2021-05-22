#include "MouseDriver.h"

void printf(char *);



MouseEventHandler::MouseEventHandler()
{
}
void MouseEventHandler::onActivate()
{
}
void MouseEventHandler::onMouseDown(uint8_t button)
{
}
void MouseEventHandler::onMouseUp(uint8_t button)
{
}
void MouseEventHandler::onMouseMove(int delta_x, int delta_y)
{
}

// Mouse uses the same data port and command port as keyboard
MouseDriver::MouseDriver(InterruptManager *interrupt_manager, MouseEventHandler* handler)
    : InterruptHandler(0x2C, interrupt_manager),
      data_port(0x60),
      command_port(0x64),
      offset(0U),
      buttons(0U),
      handler(handler)
{
}

void MouseDriver::activate()
{
    // when you start operating system, this will just desc
    while (command_port.read() & 0x1)
    {
        data_port.read();
    }
    command_port.write(0xA8); // activate interrupts
    command_port.write(0x20); // get current state
    uint8_t status = data_port.read() | 2;
    command_port.write(0x60); // set state
    data_port.write(status);
    
    command_port.write(0xD4);
    data_port.write(0xF4); // activate mouse
    data_port.read();
}

uint32_t MouseDriver::handleInterrupt(uint32_t esp)
{
    // check command port status, if the sixth bit is not 1, there is no data to read
    uint8_t status = command_port.read();
    if ((status & 0x20) == 0 || handler == nullptr)
    {
        return esp;
    }
    buffer[offset] = data_port.read();
    offset = (offset + 1) % 3;
    if (offset == 0) // now we have all 3 bytes and we can make mova a cursor
    {
        if(buffer[1] != 0 || buffer[2] != 0)
        {
        handler->onMouseMove((int8_t)buffer[1], (int8_t) -buffer[2]);
        
        // TODO: I am not shure how to handle mouseUp, upDown, for now, I will assume switch from 0 to 1 as down,
        // and from 0 to 1 as one.
        for(uint8_t i = 0; i < 3; i++)
        {
            if((buffer[0] & (0x1<<i)) != (buttons & (0x1<<i)))
            {
                if(buttons & (0x1<<i))
                    handler->onMouseUp(i+1);
                else
                    handler->onMouseDown(i+1);
            }
        }
        buttons = buffer[0];
        }
    }   

    return esp;
}

MouseDriver::~MouseDriver() {}
