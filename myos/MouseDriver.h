#ifndef MOUSEDRIVER_H
#define MOUSEDRIVER_H

#include "InterruptManager.h"
#include "Driver.h"

class MouseEventHandler
{
public:
    MouseEventHandler();

    virtual void onActivate();
    virtual void onMouseDown(uint8_t button);
    virtual void onMouseUp(uint8_t button);
    virtual void onMouseMove(int delta_x, int delta_y);
};

class MouseDriver: public InterruptHandler, public Driver
{
    Port8bit data_port;
    Port8bit command_port;

    // for each mouse command we get 3 bytes, but tricky part is to catch the first one for some reason.
    // that is why we use this offset to figure out that first byte 
    uint8_t buffer[3];
    uint8_t offset; 
    uint8_t buttons;
    MouseEventHandler* handler;
public:
    MouseDriver(InterruptManager* interrupt_manager, MouseEventHandler *handler=nullptr);
    ~MouseDriver();
    virtual uint32_t handleInterrupt(uint32_t esp) override;
    virtual void activate() override;
};

#endif // MOUSEDRIVER_H
