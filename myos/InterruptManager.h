#ifndef INTERRUPTMANAGER_H
#define INTERRUPTMANAGER_H

#include "types.h"
#include "Port.h"
#include "GlobalDescriptorTable.h"

class InterruptManager;

// TODO: separate this later to another file
class InterruptHandler
{
protected:
    uint8_t interrupt_number;
    InterruptManager *interrupt_manager;
    InterruptHandler(uint8_t interrupt_number, InterruptManager *interrupt_manager);
    // can not make it virtual, but for now it will work, as derived handlers only use base class destructor
    // check https://stackoverflow.com/questions/7015285/undefined-reference-to-operator-deletevoid
    ~InterruptHandler();

public:
    virtual uint32_t handleInterrupt(uint32_t esp);
};

//http://www.lowlevel.eu/wiki/T%C3%BDndur
class InterruptManager
{
    friend class InterruptHandler;

protected:
    struct GateDescriptor
    {
        uint16_t handlerAddressLowBits;
        uint16_t gdt_codeSegmentSelector;
        uint8_t reserved;
        uint8_t access;
        uint16_t handlerAddressHighBits;
    } __attribute__((packed));

    struct InterruptDescriptorTablePointer
    {
        uint16_t size;
        uint32_t base;
    } __attribute__((packed));

    static InterruptManager *activeInterruptManager;
    static GateDescriptor interruptDescriptorTable[256];
    InterruptHandler *interruptHandlers[256];
    Port8bitSlow picMasterCommand;
    Port8bitSlow picMasterData;
    Port8bitSlow picSlaveCommand;
    Port8bitSlow picSlaveData;

    static void setInterruptDescriptorTableEntry(
        uint8_t interruptNumber,
        uint16_t codeSegmentSelectorOffset,
        void (*handler)(),
        uint8_t descriptorPrivilegeLevel,
        uint8_t descriptorType);

public:
    InterruptManager(GlobalDescriptorTable *gdt);
    ~InterruptManager() = default;

    static uint32_t handleInterrupt(uint8_t interrupt_number, uint32_t esp);
    // to be able to use members picMasterCommand, we need to use objects for this
    // that is why we make static InterruptManager* activeManager and just call handleInterruptMember
    // from handleInterrupt
    uint32_t handleInterruptMember(uint8_t interrupt_number, uint32_t esp);

    static void ignoreInterruptRequest();
    static void handleInterruptRequest0x00();
    static void handleInterruptRequest0x01();
    static void handleInterruptRequest0x0C();

    void activate();
    void deactivate();
};

#endif // INTERRUPTMANAGER_H
