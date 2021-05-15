#include "InterruptManager.h"

void printf(char *str);

InterruptManager::GateDescriptor InterruptManager::interruptDescriptorTable[256];

uint32_t InterruptManager::handleInterrupt(uint8_t interrupt_number, uint32_t esp)
{
    printf("INTERRUPT!!!\n");
    // return current stack pointer
    return esp;
}

InterruptManager::InterruptManager(GlobalDescriptorTable *gdt)
: picMasterCommand(0x20),
  picMasterData(0x21),
  picSlaveCommand(0xA0),
  picSlaveData(0xA1)
{
    uint16_t codeSegment = gdt->get_code_segment_address_offset();
    const uint8_t IDT_INTERRUPT_GATE = 0xE;

    for (uint16_t i = 0; i < 256; i++)
    {
        setInterruptDescriptorTableEntry(i, codeSegment, &ignoreInterruptRequest, 0, IDT_INTERRUPT_GATE); // descriptorPrivilegeLevel=0 kernel states
    }

    setInterruptDescriptorTableEntry(0x20, codeSegment, &handleInterruptRequest0x00, 0, IDT_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(0x21, codeSegment, &handleInterruptRequest0x01, 0, IDT_INTERRUPT_GATE);

    // Before loading Interrupt Descriptor Table, we communicate with master and slave rogrammable interrupt controller 
    picMasterCommand.write(0x11);
    picSlaveCommand.write(0x11);
    
    // when pressing keyboard, we get interrupt 1, but the same number is used internally for exceptions also
    // we tell the master pic that for every interrupt, just add 0x20(0x20...0x27) to it(same for slave, add 0x28) 
    picMasterData.write(0x20);
    picSlaveData.write(0x28);

    // tell master he is a master, and the slave that he is the slave
    picMasterData.write(0x04);
    picSlaveData.write(0x02);

    picMasterData.write(0x01);
    picSlaveData.write(0x01);
    
    picMasterData.write(0x00);
    picSlaveData.write(0x00);

    InterruptDescriptorTablePointer idtp;
    idtp.size = 256 * sizeof(GateDescriptor) - 1;
    idtp.base = (uint32_t)interruptDescriptorTable;
    // tell the processor to load interrupt descriptor table
    asm volatile("lidt %0"
                 :
                 : "m"(idtp));
    printf("Loaded Interrupt Descriptor Table\n");
}

void InterruptManager::setInterruptDescriptorTableEntry(
    uint8_t interruptNumber,
    uint16_t codeSegmentSelectorOffset,
    void (*handler)(),
    uint8_t descriptorPrivilegeLevel,
    uint8_t descriptorType)
{
    const uint8_t IDT_DESC_PRESENT = 0x80;
    interruptDescriptorTable[interruptNumber].handlerAddressLowBits = ((uint32_t)handler) & 0xFFFF;
    interruptDescriptorTable[interruptNumber].handlerAddressHighBits = (((uint32_t)handler) >> 16) & 0xFFFF;
    interruptDescriptorTable[interruptNumber].gdt_codeSegmentSelector = codeSegmentSelectorOffset;
    interruptDescriptorTable[interruptNumber].access = IDT_DESC_PRESENT | descriptorType | ((descriptorPrivilegeLevel & 3) << 5);
    interruptDescriptorTable[interruptNumber].reserved = 0;
}

void InterruptManager::activate()
{
    // start interrupts
    asm("sti"); 
}