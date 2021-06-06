#include <hardware_communication/InterruptManager.h>
#include <common/print_to_console.h>

using namespace myos;
using namespace myos::common;
using namespace myos::hardware_communication;

InterruptManager::GateDescriptor InterruptManager::interruptDescriptorTable[256];

InterruptManager *InterruptManager::activeInterruptManager{nullptr};

uint32_t InterruptManager::handleInterrupt(uint8_t interrupt_number, uint32_t esp)
{
    if (activeInterruptManager != nullptr)
    {
        return activeInterruptManager->handleInterruptMember(interrupt_number, esp);
    }
    // otherwise just return current stack pointer
    return esp;
}

uint32_t InterruptManager::handleInterruptMember(uint8_t interrupt_number, uint32_t esp)
{
    if (interruptHandlers[interrupt_number] != nullptr)
    {
        esp = interruptHandlers[interrupt_number]->handleInterrupt(esp);
    }
    else if (interrupt_number != 0x20)
    {
        printf("UNHANDLED INTERRUPT ");
        printfHex(interrupt_number);
    }

    if(interrupt_number == 0x20U) // timer intrerrupt
    {
        esp = (uint32_t)task_manager->schedule((CPUstate*)esp);
    }

    // if this was hardware interrupt, send the response to pics
    if (interrupt_number >= 0x20 && interrupt_number < 0x30)
    {
        // always send respond to master pic, and to slave only if interrupt came from slave(>=0x28)
        picMasterCommand.write(0x20);
        if (interrupt_number >= 0x28)
        {
            picSlaveCommand.write(0x20);
        }
    }
    return esp;
}

InterruptManager::InterruptManager(GlobalDescriptorTable *gdt, TaskManager *task_manager)
    : picMasterCommand(0x20),
      picMasterData(0x21),
      picSlaveCommand(0xA0),
      picSlaveData(0xA1),
      task_manager(task_manager)
{
    uint16_t codeSegment = gdt->getCodeSegmentAddressOffset();
    const uint8_t IDT_INTERRUPT_GATE = 0xE;

    for (uint16_t i = 0; i < 256; i++)
    {
        setInterruptDescriptorTableEntry(i, codeSegment, &ignoreInterruptRequest, 0, IDT_INTERRUPT_GATE); // descriptorPrivilegeLevel=0 kernel states
        interruptHandlers[i] = nullptr;
    }

    setInterruptDescriptorTableEntry(0x20, codeSegment, &handleInterruptRequest0x00, 0, IDT_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(0x21, codeSegment, &handleInterruptRequest0x01, 0, IDT_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(0x22, codeSegment, &handleInterruptRequest0x02, 0, IDT_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(0x23, codeSegment, &handleInterruptRequest0x03, 0, IDT_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(0x24, codeSegment, &handleInterruptRequest0x04, 0, IDT_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(0x25, codeSegment, &handleInterruptRequest0x05, 0, IDT_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(0x26, codeSegment, &handleInterruptRequest0x06, 0, IDT_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(0x27, codeSegment, &handleInterruptRequest0x07, 0, IDT_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(0x28, codeSegment, &handleInterruptRequest0x08, 0, IDT_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(0x29, codeSegment, &handleInterruptRequest0x09, 0, IDT_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(0x2A, codeSegment, &handleInterruptRequest0x0A, 0, IDT_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(0x2B, codeSegment, &handleInterruptRequest0x0B, 0, IDT_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(0x2C, codeSegment, &handleInterruptRequest0x0C, 0, IDT_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(0x2D, codeSegment, &handleInterruptRequest0x0D, 0, IDT_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(0x2E, codeSegment, &handleInterruptRequest0x0E, 0, IDT_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(0x2F, codeSegment, &handleInterruptRequest0x0F, 0, IDT_INTERRUPT_GATE);
    setInterruptDescriptorTableEntry(0x80U, codeSegment, &handleInterruptRequest0x80, 0, IDT_INTERRUPT_GATE);


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
    if (activeInterruptManager != nullptr)
    {
        activeInterruptManager->deactivate();
    }
    activeInterruptManager = this;
    // start interrupts
    asm("sti");
}

void InterruptManager::deactivate()
{
    if (activeInterruptManager == this)
    {
        activeInterruptManager = nullptr;
        // stop interrupts
        asm("cli");
    }
}

InterruptHandler::InterruptHandler(uint8_t interrupt_number, InterruptManager *interrupt_manager)
    : interrupt_number(interrupt_number),
      interrupt_manager(interrupt_manager)
{
    interrupt_manager->interruptHandlers[interrupt_number] = this;
}

InterruptHandler::~InterruptHandler()
{
    if (interrupt_manager->interruptHandlers[interrupt_number] == this)
    {
        interrupt_manager->interruptHandlers[interrupt_number] = nullptr;
    }
}

uint32_t InterruptHandler::handleInterrupt(uint32_t esp)
{
    return esp;
}
