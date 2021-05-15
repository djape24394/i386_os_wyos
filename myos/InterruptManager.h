#ifndef INTERRUPTMANAGER_H
#define INTERRUPTMANAGER_H

#include "types.h"
#include "Port.h"
#include "GlobalDescriptorTable.h"

//http://www.lowlevel.eu/wiki/T%C3%BDndur
class InterruptManager
{
protected:
    struct GateDescriptor
    {
        uint16_t handlerAddressLowBits;
        uint16_t gdt_codeSegmentSelector; 
        uint8_t reserved;
        uint8_t access;
        uint16_t handlerAddressHighBits;
    }__attribute__((packed));
    struct InterruptDescriptorTablePointer
    {
        uint16_t size;
        uint32_t base;
    }__attribute__((packed));
    static GateDescriptor interruptDescriptorTable[256];
    static void setInterruptDescriptorTableEntry(
        uint8_t interruptNumber,
        uint16_t codeSegmentSelectorOffset,
        void (*handler)(),
        uint8_t descriptorPrivilegeLevel,
        uint8_t descriptorType);
    Port8bitSlow picMasterCommand;
    Port8bitSlow picMasterData;
    Port8bitSlow picSlaveCommand;
    Port8bitSlow picSlaveData;
public:
    InterruptManager(GlobalDescriptorTable* gdt);
    ~InterruptManager() = default;
    static uint32_t handleInterrupt(uint8_t interrupt_number, uint32_t esp);
    
    static void ignoreInterruptRequest();
    static void handleInterruptRequest0x00();
    static void handleInterruptRequest0x01();

    void activate();
};

#endif // INTERRUPTMANAGER_H