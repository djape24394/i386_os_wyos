#ifndef MYOS_DRIVERS_AMD_AM79C973_H
#define MYOS_DRIVERS_AMD_AM79C973_H

#include <common/types.h>
#include <drivers/Driver.h>
#include <hardware_communication/pci.h>
#include <hardware_communication/InterruptManager.h>
#include <hardware_communication/Port.h>

namespace myos
{
    namespace drivers
    {
        class AMD_am79c973 : public Driver, public hardware_communication::InterruptHandler
        {
            struct InitializationBlock
            {
                common::int16_t mode;
                unsigned reserved1 : 4;
                unsigned nof_send_buffers : 4;
                unsigned reserved2 : 4;
                unsigned nof_receving_buffers : 4;
                common::uint64_t physical_address : 48;
                common::uint16_t reserved3;
                common::uint64_t logical_address;
                common::uint32_t receive_buffer_descriptor_address;
                common::uint32_t send_buffer_descriptor_address;
            } __attribute__((packed));
            struct BufferDescriptor
            {
                common::uint32_t address;
                common::uint32_t flags;
                common::uint32_t flags2;
                common::uint32_t avail;
            } __attribute__((packed));

            hardware_communication::Port16bit mac_address_port_0;
            hardware_communication::Port16bit mac_address_port_2;
            hardware_communication::Port16bit mac_address_port_4;
            hardware_communication::Port16bit register_data_port;
            hardware_communication::Port16bit register_address_port;
            hardware_communication::Port16bit reset_port;
            hardware_communication::Port16bit bus_control_register_data_port;

            InitializationBlock initialization_block;

            BufferDescriptor *send_buffer_descriptor_pointer;
            common::uint8_t send_buffer_descriptor_memory[2048 + 15];
            common::uint8_t send_buffers[2 * 1024 + 15][8]; // 8 buffers of size 2KB + 15B for allignment
            common::uint8_t current_send_buffer_index;

            BufferDescriptor *receive_buffer_descriptor_pointer;
            common::uint8_t receive_buffer_descriptor_memory[2048 + 15];
            common::uint8_t recieve_buffers[2 * 1024 + 15][8]; // 8 buffers of size 2KB + 15B for allignment
            common::uint8_t current_receive_buffer_index;
        public:
            AMD_am79c973(myos::hardware_communication::PeripheralComponentInterconnectDeviceDescriptor *pci_descriptor, myos::hardware_communication::InterruptManager *interrupt_manager);
            ~AMD_am79c973();

            virtual void activate() override;
            virtual int reset() override;

            virtual myos::common::uint32_t handleInterrupt(myos::common::uint32_t esp) override;

            void send_data(common::uint8_t* buffer, int size);
            void receive_data();

        };
    } // namespace drivers
} // namespace myos

#endif // MYOS_DRIVERS_AMD_AM79C973_H
