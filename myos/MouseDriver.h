#ifndef MOUSEDRIVER_H
#define MOUSEDRIVER_H

#include "InterruptManager.h"

class MouseDriver: public InterruptHandler
{
    Port8bit data_port;
    Port8bit command_port;

    // for each mouse command we get 3 bytes, but tricky part is to catch the first one for some reason.
    // that is why we use this offset to figure out that first byte 
    uint8_t buffer[3];
    uint8_t offset; 
    uint8_t buttons;
public:
    MouseDriver(InterruptManager* interrupt_manager);
    ~MouseDriver();
    virtual uint32_t handleInterrupt(uint32_t esp) override;
};

#endif // MOUSEDRIVER_H
