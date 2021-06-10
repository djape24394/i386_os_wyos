#ifndef MYOS_GLOBALDESCRIPTORTABLE_H
#define MYOS_GLOBALDESCRIPTORTABLE_H

#include <common/types.h>

namespace myos
{
        /**
         * Global descriptor table 
         * https://en.wikipedia.org/wiki/Global_Descriptor_Table
         * https://wiki.osdev.org/GDT_Tutorial
         * https://wiki.osdev.org/Global_Descriptor_Table
         */
    class GlobalDescriptorTable
    {
    public:
        /**
         * Nested class for etries, 8 bytes per entry and each entry is organised as below.
         * [[base_vhi], [flags_limi_hi|flags_limi_lo], [type], [base_hi] ,[base_lo]x2, [limit_lo]x2]
         * As you can see, there are pieces of pointer and limit everywhere, because of backward compatibility.
         */
        class SegmentDescriptor
        {
        private:
            common::uint16_t limit_low_bytes; // limit low bytes
            common::uint16_t base_low_bytes;  // lower 2 bytes of base pointer
            common::uint8_t base_high_byte;   // higher byte of base pointer at byte 3
            common::uint8_t type;             // access rights
            common::uint8_t flags_limit;      // upper 4 bits for flags, lower 4 bits for limit highest 4 bytes
            common::uint8_t base_vhi;         // base part at the byte index 7
        public:
            SegmentDescriptor(common::uint32_t base_ptr, common::uint32_t limit, common::uint8_t type);
            common::uint32_t getBasePointer();
            common::uint32_t getLimitSize();
        } __attribute__((packed)); // needs to be byte perfect, all member are aligned next to each other

        // By default we must use one empty and one unused. 
        // We have only one code and one data segment for now, and they both spread through entire memory.
        SegmentDescriptor nullSegmentSelector;
        SegmentDescriptor unusedSegmentSelector;
        SegmentDescriptor codeSegmentSelector;
        SegmentDescriptor dataSegmentSelector;
        GlobalDescriptorTable();
        ~GlobalDescriptorTable();

        common::uint16_t getCodeSegmentAddressOffset(); //offset of the code segment descrtiptor in bytes
        common::uint16_t getDataSegmentAddressOffset(); //offset of the data segment descrtiptor in bytes
    };
} // namespace myos

#endif // MYOS_GLOBALDESCRIPTORTABLE_H
