#include <GlobalDescriptorTable.h>
#include <common/print_to_console.h>

using namespace myos::common;
using namespace myos;

GlobalDescriptorTable::GlobalDescriptorTable()
    : nullSegmentSelector(0U, 0U, 0U),
      unusedSegmentSelector(0u, 0u, 0u),
      codeSegmentSelector(0u, 64 * 1024 * 1024, 0x9A), // 64MB and flags, 0x9A taken from the link
      dataSegmentSelector(0u, 64 * 1024 * 1024, 0x92)  // 64MB and flags
{
    // tell the processor to use this table, processor expects 6 bytes in row, 2 bytes are sizeof table, 4 bytes are address
    uint32_t i[2];
    i[0] = sizeof(GlobalDescriptorTable) << 16; // need to push that to higher 2 bytes, to have 6 bytes in row
    i[1] = (uint32_t)this;

    // execute assembler code to tell the processor to use this descriptor table
    // lgdt: "Load Global Descriptor table"
    // https://www.codeproject.com/Articles/15971/Using-Inline-Assembly-in-C-C
    asm volatile("lgdt (%0)"
                 :
                 : "p"(((uint8_t *)i) + 2));
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
    // limit use firs 2.5 bytes, rest must be all ones
    // TODO: refactor this function to use SegmentDescriptor variables as below
    uint8_t *target = (uint8_t *)this;
    // encode limit
    if (limit <= 65536)
    {
        target[6] = 0x40;
    }
    else
    {
        if ((limit & 0xFFF) != 0xFFF)
        {
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
    if (flags_limit & 0xC0 != 0xC0)
        limit = (limit << 12) | 0xFFF;
    return limit;
}
