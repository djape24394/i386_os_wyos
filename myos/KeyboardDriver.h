#ifndef KEYBOARDDRIVER_H
#define KEYBOARDDRIVER_H

#include "InterruptManager.h"

class KeyboardDriver: public InterruptHandler
{
    Port8bit data_port;
    Port8bit command_port;
public:
    KeyboardDriver(InterruptManager* interrupt_manager);
    ~KeyboardDriver();
    virtual uint32_t handleInterrupt(uint32_t esp) override;
};

#endif // KEYBOARDDRIVER_H
