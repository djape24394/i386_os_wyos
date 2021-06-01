#ifndef MYOS_SYSTEM_CALLS_H
#define MYOS_SYSTEM_CALLS_H

#include <hardware_communication/InterruptManager.h>
#include <common/types.h>
#include <hardware_communication/InterruptManager.h>
#include <multitasking.h>

namespace myos
{
    class SystemCallHandler: public hardware_communication::InterruptHandler
    {
    public:
        SystemCallHandler(hardware_communication::InterruptManager *interrupt_manager, common::uint8_t interrupt_number);
        ~SystemCallHandler();

        virtual myos::common::uint32_t handleInterrupt(myos::common::uint32_t esp) override;

    };
}

#endif
