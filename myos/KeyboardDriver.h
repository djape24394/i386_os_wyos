#ifndef KEYBOARDDRIVER_H
#define KEYBOARDDRIVER_H

#include "InterruptManager.h"
#include "Driver.h"

class KeyboardEventHandler
{
public:
    KeyboardEventHandler();

    virtual void onKeyDown(char);
    virtual void onKeyUp(char);
};

class KeyboardDriver: public InterruptHandler, public Driver
{
    Port8bit data_port;
    Port8bit command_port;

    KeyboardEventHandler *handler;
public:
    KeyboardDriver(InterruptManager* interrupt_manager, KeyboardEventHandler* event_handler=nullptr);
    ~KeyboardDriver();
    virtual uint32_t handleInterrupt(uint32_t esp) override;
    virtual void activate()override;
};

#endif // KEYBOARDDRIVER_H
