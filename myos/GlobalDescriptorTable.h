#ifndef GDT_H
#define GDT_H

#include "types.h"

/*! 
Global descriptor table 
https://en.wikipedia.org/wiki/Global_Descriptor_Table
https://wiki.osdev.org/GDT_Tutorial
https://wiki.osdev.org/Global_Descriptor_Table
*/
class GlobalDescriptorTable
{
public:
    /*
    nested class for etries, 8 bytes
    Descriptopr
    [[base_vhi], [flags_limi_hi|flags_limi_lo], [type], [base_hi] ,[base_lo]x2, [limit_lo]x2]
    */
    class SegmentDescriptor
    {
    private:
        uint16_t limit_low_bytes; // limit low bytes
        uint16_t base_low_bytes;  // lower 2 bytes of base pointer
        uint8_t base_high_byte;   // higher byte of base pointer at byte 3
        uint8_t type;             // access rights
        uint8_t flags_limit;      // upper 4 bits for flags, lower 4 bits for limit
        uint8_t base_vhi;         // base part at the byte index 7
    public:
        SegmentDescriptor(uint32_t base_ptr, uint32_t limit, uint8_t type);
        uint32_t get_base_ptr();
        uint32_t get_limit_size();
    } __attribute__((packed)); // needs to be byte perfect, all member are aligned next to each other

    SegmentDescriptor nullSegmentSelector; 
    SegmentDescriptor unusedSegmentSelector;
    SegmentDescriptor codeSegmentSelector;
    SegmentDescriptor dataSegmentSelector;
    GlobalDescriptorTable();
    ~GlobalDescriptorTable();

    uint16_t get_code_segment_address_offset(); //offset of the code segment descrtiptor
    uint16_t get_data_segment_address_offset(); //offset of the data segment descrtiptor
};

#endif
