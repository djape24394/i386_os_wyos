#ifndef MYOS_HARDWARE_COMMUNICATION_INTERRUPTMANAGER_H
#define MYOS_HARDWARE_COMMUNICATION_INTERRUPTMANAGER_H

#include <common/types.h>
#include <hardware_communication/Port.h>
#include <GlobalDescriptorTable.h>
#include <multitasking.h>

namespace myos::hardware_communication
{
    class InterruptManager;

    // TODO: separate this later to another file
    class InterruptHandler
    {
    protected:
        myos::common::uint8_t interrupt_number;
        InterruptManager *interrupt_manager;
        InterruptHandler(myos::common::uint8_t interrupt_number, InterruptManager *interrupt_manager);
        // can not make it virtual, but for now it will work, as derived handlers only use base class destructor
        // check https://stackoverflow.com/questions/7015285/undefined-reference-to-operator-deletevoid
        ~InterruptHandler();

    public:
        virtual myos::common::uint32_t handleInterrupt(myos::common::uint32_t esp);
    };

    //http://www.lowlevel.eu/wiki/T%C3%BDndur
    class InterruptManager
    {
        friend class InterruptHandler;

    protected:
        struct GateDescriptor
        {
            myos::common::uint16_t handlerAddressLowBits;
            myos::common::uint16_t gdt_codeSegmentSelector; // offset from the GDT address, GDT address already introduced with lgdt function in GDT constructor
            myos::common::uint8_t reserved;
            myos::common::uint8_t access; // access rights
            myos::common::uint16_t handlerAddressHighBits;
        } __attribute__((packed));

        struct InterruptDescriptorTablePointer
        {
            myos::common::uint16_t size;
            myos::common::uint32_t base;
        } __attribute__((packed));

        static InterruptManager *activeInterruptManager;
        static GateDescriptor interruptDescriptorTable[256];
        InterruptHandler *interruptHandlers[256];
        myos::TaskManager *task_manager;
        Port8bitSlow picMasterCommand;
        Port8bitSlow picMasterData;
        Port8bitSlow picSlaveCommand;
        Port8bitSlow picSlaveData;

        static void setInterruptDescriptorTableEntry(
            myos::common::uint8_t interruptNumber,
            myos::common::uint16_t codeSegmentSelectorOffset,
            void (*handler)(),
            myos::common::uint8_t descriptorPrivilegeLevel,
            myos::common::uint8_t descriptorType);

    public:
        InterruptManager(GlobalDescriptorTable *gdt, myos::TaskManager *task_manager);
        ~InterruptManager() = default;

        static myos::common::uint32_t handleInterrupt(myos::common::uint8_t interrupt_number, myos::common::uint32_t esp);
        // to be able to use members picMasterCommand, we need to use objects for this
        // that is why we make static InterruptManager* activeManager and just call handleInterruptMember
        // from handleInterrupt
        myos::common::uint32_t handleInterruptMember(myos::common::uint8_t interrupt_number, myos::common::uint32_t esp);

        static void ignoreInterruptRequest();

        static void handleInterruptRequest0x00();
        static void handleInterruptRequest0x01();
        static void handleInterruptRequest0x02();
        static void handleInterruptRequest0x03();
        static void handleInterruptRequest0x04();
        static void handleInterruptRequest0x05();
        static void handleInterruptRequest0x06();
        static void handleInterruptRequest0x07();
        static void handleInterruptRequest0x08();
        static void handleInterruptRequest0x09();
        static void handleInterruptRequest0x0A();
        static void handleInterruptRequest0x0B();
        static void handleInterruptRequest0x0C();
        static void handleInterruptRequest0x0D();
        static void handleInterruptRequest0x0E();
        static void handleInterruptRequest0x0F();
        static void handleInterruptRequest0x31();
        static void handleInterruptRequest0x80();

        void activate();
        void deactivate();
    };
} // namespace myos::hardware_communication

#endif // MYOS_HARDWARE_COMMUNICATION_INTERRUPTMANAGER_H
