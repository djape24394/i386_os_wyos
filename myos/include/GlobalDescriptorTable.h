#ifndef MYOS_GLOBALDESCRIPTORTABLE_H
#define MYOS_GLOBALDESCRIPTORTABLE_H

#include <common/types.h>

namespace myos
{
        /**
         *Global descriptor table 
         *https://en.wikipedia.org/wiki/Global_Descriptor_Table
         *https://wiki.osdev.org/GDT_Tutorial
         *https://wiki.osdev.org/Global_Descriptor_Table
         */
    class GlobalDescriptorTable
    {
    public:
        /**
             *nested class for etries, 8 bytes
             *Descriptopr
             *[[base_vhi], [flags_limi_hi|flags_limi_lo], [type], [base_hi] ,[base_lo]x2, [limit_lo]x2]
             */
        class SegmentDescriptor
        {
        private:
            myos::common::uint16_t limit_low_bytes; // limit low bytes
            myos::common::uint16_t base_low_bytes;  // lower 2 bytes of base pointer
            myos::common::uint8_t base_high_byte;   // higher byte of base pointer at byte 3
            myos::common::uint8_t type;             // access rights
            myos::common::uint8_t flags_limit;      // upper 4 bits for flags, lower 4 bits for limit
            myos::common::uint8_t base_vhi;         // base part at the byte index 7
        public:
            SegmentDescriptor(myos::common::uint32_t base_ptr, myos::common::uint32_t limit, myos::common::uint8_t type);
            myos::common::uint32_t getBasePointer();
            myos::common::uint32_t getLimitSize();
        } __attribute__((packed)); // needs to be byte perfect, all member are aligned next to each other

        SegmentDescriptor nullSegmentSelector;
        SegmentDescriptor unusedSegmentSelector;
        SegmentDescriptor codeSegmentSelector;
        SegmentDescriptor dataSegmentSelector;
        GlobalDescriptorTable();
        ~GlobalDescriptorTable();

        myos::common::uint16_t getCodeSegmentAddressOffset(); //offset of the code segment descrtiptor
        myos::common::uint16_t getDataSegmentAddressOffset(); //offset of the data segment descrtiptor
    };
} // namespace myos

#endif // MYOS_GLOBALDESCRIPTORTABLE_H
