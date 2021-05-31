#include <drivers/AdvancedTechnologyAttachment.h>

using namespace myos;
using namespace myos::common;
using namespace myos::drivers;
using namespace myos::hardware_communication;

void printf(char *);

AdvancedTechnologyAttachment::AdvancedTechnologyAttachment(uint16_t port_base, bool master)
    : data_port(port_base),
      error_port(port_base + 1U),
      sector_count_port(port_base + 2U),
      lba_low_port(port_base + 3U),
      lba_mid_port(port_base + 4U),
      lba_high_port(port_base + 5U),
      device_port(port_base + 6U),
      command_port(port_base + 7U),
      control_port(port_base + 0x206U),
      master(master),
      bytes_per_sector(512U)
{
}

AdvancedTechnologyAttachment::~AdvancedTechnologyAttachment()
{
}

void AdvancedTechnologyAttachment::identify()
{
    device_port.write(master ? 0xA0U : 0xB0U); // I want to talk to master or slave
    control_port.write(0);

    device_port.write(0xA0U);
    // read the status of the master
    uint8_t status = command_port.read();
    if (status == 0xFF)
        return; // no device in that bus

    device_port.write(master ? 0xA0U : 0xB0U);
    sector_count_port.write(0);
    lba_low_port.write(0U);
    lba_mid_port.write(0U);
    lba_high_port.write(0U);

    command_port.write(0xEC); // command for identify
    status = command_port.read();
    if (status == 0x00U)
        return; // no device
    
    // it may take some time for the hard drive to give some answer to indentify command, so wait for the status
    while ( ((status & 0x80U) == 0x80U) // 0x80U means its busy
        &&  ((status & 0x01U) != 0x01U) ) // 0x01 means error happened
    {
        status = command_port.read();
    }

    if(status & 0x01U)
    {
        printf("ERROR");
        return;
    }

    // no error, it means we got the data
    for(uint16_t i = 0U; i < 256U; i++)
    {
        uint16_t data = data_port.read();
        char *foo = "  \0";
        foo[1] = (data >> 8) & 0x00FFU;
        foo[0] = data & 0x00FFU;
        printf(foo);
    }
}

void AdvancedTechnologyAttachment::read28(uint32_t sector, uint8_t *data, int count)
{
    // sector can not be addressed with more than 28 bits
    if(sector & 0xF0000000U) return;
    if(count > bytes_per_sector) return;

    device_port.write((master ? 0xE0U : 0xF0U) | ((sector & 0x0F000000U) >> 24U)); // I want to talk to master or slave
    error_port.write(0U); // clear error
    sector_count_port.write(1U); // for now we always read/write single sector

    lba_low_port.write(sector && 0x000000FFU);
    lba_mid_port.write((sector && 0x0000FF00U) >> 8U);
    lba_high_port.write((sector && 0x00FF0000U) >> 16U);

    command_port.write(0x20U); // command for write
    

    uint8_t status = command_port.read();
    // it may take some time for the hard drive to fill the buffer, wait
    while ( ((status & 0x80U) == 0x80U) // 0x80U means its busy
        &&  ((status & 0x01U) != 0x01U) ) // 0x01 means error happened
    {
        status = command_port.read();
    }

    if(status & 0x01U)
    {
        printf("ERROR");
        return;
    }

    printf("Read from ATA: ");

    for(uint16_t i = 0U; i < count; i += 2)
    {
        uint16_t wdata = data_port.read();
        char *foo = "  \0";
        foo[1] = (wdata >> 8) & 0x00FFU;
        foo[0] = wdata & 0x00FFU;
        printf(foo);

        data[i] = wdata & 0x00FFU;
        if(i + 1 < count) data[i + 1] = (wdata >> 8) & 0x00FFU;
    }
    // we need to always read the whole sector
    for(uint16_t i = count + (count % 2); i < bytes_per_sector; i += 2)
    {
        (void) data_port.read();
    }
}

void AdvancedTechnologyAttachment::write28(uint32_t sector, uint8_t *data, int count)
{
    // sector can not be addressed with more than 28 bits
    if(sector & 0xF0000000U) return;
    if(count > bytes_per_sector) return;

    device_port.write((master ? 0xE0U : 0xF0U) | ((sector & 0x0F000000U) >> 24U)); // I want to talk to master or slave
    error_port.write(0U); // clear error
    sector_count_port.write(1U); // for now we always read/write single sector

    lba_low_port.write(sector && 0x000000FFU);
    lba_mid_port.write((sector && 0x0000FF00U) >> 8U);
    lba_high_port.write((sector && 0x00FF0000U) >> 16U);

    command_port.write(0x30U); // command for write
    
    printf("Write to ATA: ");
    
    for(uint16_t i = 0U; i < count; i += 2)
    {
        uint16_t wdata = data[i];
        if(i + 1 < count) wdata |= ((uint16_t)data[i + 1]) << 8U;
        char *foo = "  \0";
        foo[1] = (wdata >> 8) & 0x00FFU;
        foo[0] = wdata & 0x00FFU;
        printf(foo);

        data_port.write(wdata);
    }
    // we need to always write the whole sector
    for(uint16_t i = count + (count % 2); i < bytes_per_sector; i += 2)
    {
        data_port.write(0x0000U);
    }
}

void AdvancedTechnologyAttachment::flush()
{
    device_port.write(master ? 0xE0U : 0xF0U); // I want to talk to master or slave
    command_port.write(0xE7U); // flush

    uint8_t status = command_port.read();
    // wait while device is flushing
    while ( ((status & 0x80U) == 0x80U) // 0x80U means its busy
        &&  ((status & 0x01U) != 0x01U) ) // 0x01 means error happened
    {
        status = command_port.read();
    }

    if(status & 0x01U)
    {
        printf("ERROR");
        return;
    }
}
