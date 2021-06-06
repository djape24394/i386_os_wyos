#ifndef MYOS_DRIVERS_KEYBOARDDRIVER_H
#define MYOS_DRIVERS_KEYBOARDDRIVER_H

#include <hardware_communication/InterruptManager.h>
#include <drivers/Driver.h>

namespace myos::drivers
{
    class KeyboardEventHandler
    {
    public:
        KeyboardEventHandler();

        virtual void onKeyDown(char);
        virtual void onKeyUp(char);
    };

    class KeyboardDriver : public myos::hardware_communication::InterruptHandler, public Driver
    {
        myos::hardware_communication::Port8bit data_port;
        myos::hardware_communication::Port8bit command_port;
        KeyboardEventHandler *handler;

    public:
        KeyboardDriver(myos::hardware_communication::InterruptManager *interrupt_manager, KeyboardEventHandler *event_handler = nullptr);
        ~KeyboardDriver();
        virtual myos::common::uint32_t handleInterrupt(myos::common::uint32_t esp) override;
        virtual void activate() override;
    };
} // namespace myos::drivers

#endif // KEYBOARDDRIVER_H
