#ifndef MYOS_DRIVERS_ADVANCEDTECHNOLOGYATTACHMENT_H
#define MYOS_DRIVERS_ADVANCEDTECHNOLOGYATTACHMENT_H

#include <hardware_communication/Port.h>

namespace myos::drivers
{
    class AdvancedTechnologyAttachment
    {
    protected:
        hardware_communication::Port16bit data_port;        // through this port we send the data we want to write and pull the data we read
        hardware_communication::Port8bit error_port;        // read error messages
        hardware_communication::Port8bit sector_count_port; // tell hard drive how many sectors we want to read
        hardware_communication::Port8bit lba_low_port;      // 3 ports for transmitting Logical Block Address
        hardware_communication::Port8bit lba_mid_port;
        hardware_communication::Port8bit lba_high_port;
        hardware_communication::Port8bit device_port;  // whether we want to talk to master or slave, also for the part of lba when in 28 bit mode
        hardware_communication::Port8bit command_port; // transfer the intstruction: read, write to some sector
        hardware_communication::Port8bit control_port; // for status messages
        bool master;                                   // this is obsolete nowadays, before you could boot only from master drive
        common::uint16_t bytes_per_sector;

    public:
        AdvancedTechnologyAttachment(common::uint16_t port_base, bool master);
        ~AdvancedTechnologyAttachment();
        void identify();                                                         // with this we talk to the drive, what kind of drive it is, how many bytes are in the sector, etc.
        void read28(common::uint32_t sector, common::uint8_t *data, int count);  // highest 4 bits ignored
        void write28(common::uint32_t sector, common::uint8_t *data, int count); // highest 4 bits ignored
        void flush();                                                            // flush the internal buffer of the drive to the memory, if out of power and not flushed we get loss of data
    };
} // namespace myos::drivers

#endif
