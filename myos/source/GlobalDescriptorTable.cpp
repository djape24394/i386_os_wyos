#include <GlobalDescriptorTable.h>
#include <common/print_to_console.h>

using namespace myos::common;
using namespace myos;

GlobalDescriptorTable::GlobalDescriptorTable()
    : nullSegmentSelector(0U, 0U, 0U),
      unusedSegmentSelector(0u, 0u, 0u),
      codeSegmentSelector(0u, 64 * 1024 * 1024, 0x9A), // 64MB and flags, 0x9A taken from the lowlevel eu
      dataSegmentSelector(0u, 64 * 1024 * 1024, 0x92)  // 64MB and flags
{
    // tell the processor to use GDT, processor expects 6 bytes in row, first 2 bytes are sizeof table, next 4 bytes are address
    GlobalDescriptorTablePointer gdtp;
    gdtp.size = sizeof(GlobalDescriptorTable);
    gdtp.base = (uint32_t)this;

    // execute assembler code to tell the processor to use this descriptor table
    // lgdt: "Load Global Descriptor table"
    // https://www.codeproject.com/Articles/15971/Using-Inline-Assembly-in-C-C
    asm volatile("lgdt (%0)"
                 :
                 : "p"(&gdtp));
    printf("Loaded Global Descriptor Table\n");
}

GlobalDescriptorTable::~GlobalDescriptorTable() = default;

uint16_t GlobalDescriptorTable::getDataSegmentAddressOffset()
{
    return (uint8_t *)&dataSegmentSelector - (uint8_t *)this;
}

uint16_t GlobalDescriptorTable::getCodeSegmentAddressOffset()
{
    return (uint8_t *)&codeSegmentSelector - (uint8_t *)this;
}

GlobalDescriptorTable::SegmentDescriptor::SegmentDescriptor(uint32_t base_ptr, uint32_t limit, uint8_t type)
{
    // [[base_vhi], [flags_limi_hi|flags_limi_lo], [type], [base_lo] ,[base_lo]x2, [limit_lo]x2]
    uint8_t *target = (uint8_t *)this;
    // encode limit
    if (limit <= 65536)
    {
        // 16 bit address space
        target[6] = 0x40;
    }
    else
    {
        // 32 bit address space, last 12 bits must be all ones, and we only store the higher 20 bits.
        if ((limit & 0xFFF) != 0xFFF)
        {
            // if they are not all 1, we will decrease the upper 20 bits size by one, so we will loose up to 2^12=4KB 
            limit = (limit >> 12) - 1;
        }
        else
        {
            limit = (limit >> 12);
        }
        // set flags
        target[6] = 0xC0;
    }
    target[0] = limit & 0xFF;
    target[1] = (limit >> 8) & 0xFF;
    target[6] |= (limit >> 16) & 0x0F;

    // encode base pointer
    target[2] = base_ptr & 0xFF;
    target[3] = (base_ptr >> 8) & 0xFF;
    target[4] = (base_ptr >> 16) & 0xFF;
    target[7] = (base_ptr >> 24) & 0xFF;
    // access rights
    target[5] = type;
}

uint32_t GlobalDescriptorTable::SegmentDescriptor::getBasePointer()
{
    uint32_t base_ptr = 0u;
    base_ptr |= base_low_bytes;
    base_ptr |= ((uint32_t)base_high_byte) << 16;
    base_ptr |= ((uint32_t)base_vhi) << 24;
    return base_ptr;
}

uint32_t GlobalDescriptorTable::SegmentDescriptor::getLimitSize()
{
    uint32_t limit = 0u;
    limit |= limit_low_bytes;
    limit |= (((uint32_t)flags_limit) & 0x0F) << 16;
    if (flags_limit & 0xC0 == 0xC0)
        limit = (limit << 12) | 0xFFF;
    return limit;
}
