#ifndef MYOS_DRIVERS_MOUSEDRIVER_H
#define MYOS_DRIVERS_MOUSEDRIVER_H

#include <common/types.h>
#include <hardware_communication/InterruptManager.h>
#include <drivers/Driver.h>

namespace myos
{
    namespace drivers
    {
        class MouseEventHandler
        {
        public:
            MouseEventHandler();

            virtual void onActivate();
            virtual void onMouseDown(myos::common::uint8_t button);
            virtual void onMouseUp(myos::common::uint8_t button);
            virtual void onMouseMove(int delta_x, int delta_y);
        };

        class MouseDriver : public myos::hardware_communication::InterruptHandler, public Driver
        {
            myos::hardware_communication::Port8bit data_port;
            myos::hardware_communication::Port8bit command_port;

            // for each mouse command we get 3 bytes, but tricky part is to catch the first one for some reason.
            // that is why we use this offset to figure out that first byte
            myos::common::uint8_t buffer[3];
            myos::common::uint8_t offset;
            myos::common::uint8_t buttons;
            MouseEventHandler *handler;

        public:
            MouseDriver(myos::hardware_communication::InterruptManager *interrupt_manager, MouseEventHandler *handler = nullptr);
            ~MouseDriver();
            virtual myos::common::uint32_t handleInterrupt(myos::common::uint32_t esp) override;
            virtual void activate() override;
        };
    } // namespace drivers
} // namespace myos

#endif // MYOS_DRIVERS_MOUSEDRIVER_H
