#include <drivers/AMD_am79c973.h>
#include <common/print_to_console.h>

using namespace myos;
using namespace myos::common;
using namespace myos::drivers;
using namespace myos::hardware_communication;

AMD_am79c973::AMD_am79c973(PeripheralComponentInterconnectDeviceDescriptor *pci_descriptor, InterruptManager *interrupt_manager)
    : Driver(),
      InterruptHandler(pci_descriptor->interrupt_number + 0x20U, interrupt_manager),
      mac_address_port_0(pci_descriptor->port_base_number),
      mac_address_port_2(pci_descriptor->port_base_number + 0x02U),
      mac_address_port_4(pci_descriptor->port_base_number + 0x04U),
      register_data_port(pci_descriptor->port_base_number + 0x10U),
      register_address_port(pci_descriptor->port_base_number + 0x12U),
      reset_port(pci_descriptor->port_base_number + 0x14U),
      bus_control_register_data_port(pci_descriptor->port_base_number + 0x16U)
{
    current_send_buffer_index = 0;
    current_receive_buffer_index = 0;

    uint64_t mac_0 = mac_address_port_0.read() % 256U;
    uint64_t mac_1 = mac_address_port_0.read() / 256U;
    uint64_t mac_2 = mac_address_port_2.read() % 256U;
    uint64_t mac_3 = mac_address_port_2.read() / 256U;
    uint64_t mac_4 = mac_address_port_4.read() % 256U;
    uint64_t mac_5 = mac_address_port_4.read() / 256U;
    uint64_t mac = (mac_5 << 40) |
                   (mac_4 << 32) |
                   (mac_3 << 24) |
                   (mac_2 << 16) |
                   (mac_1 << 8) |
                   mac_0;
    // set device to 32 bit mode
    register_address_port.write(20U);
    bus_control_register_data_port.write(0x102U);

    // stop reset
    register_address_port.write(0);
    register_data_port.write(0x04U);

    // set the initialization block
    initialization_block.mode = 0x0000; // promiscuous mode = false
    initialization_block.reserved1 = 0;
    initialization_block.nof_send_buffers = 3;
    initialization_block.reserved2 = 0;
    initialization_block.nof_receving_buffers = 3;
    initialization_block.physical_address = mac;
    initialization_block.reserved3 = 0;
    initialization_block.logical_address = 0;

    send_buffer_descriptor_pointer = (BufferDescriptor *)(((uint32_t)(&send_buffer_descriptor_memory[0]) + 15U) & (~(0x0FU)));
    initialization_block.send_buffer_descriptor_address = (uint32_t)send_buffer_descriptor_pointer;

    receive_buffer_descriptor_pointer = (BufferDescriptor *)(((uint32_t)(&receive_buffer_descriptor_memory[0]) + 15U) & (~(0x0FU)));
    initialization_block.receive_buffer_descriptor_address = (uint32_t)receive_buffer_descriptor_pointer;

    for (uint8_t i = 0; i < 8; i++)
    {
        send_buffer_descriptor_pointer[i].address = ((uint32_t)(&send_buffers[i]) + 15U) & (~(0x0FU));
        send_buffer_descriptor_pointer[i].flags = 0x07FFU | 0xF000U;
        send_buffer_descriptor_pointer[i].flags2 = 0U;
        send_buffer_descriptor_pointer[i].avail = 0U;

        receive_buffer_descriptor_pointer[i].address = ((uint32_t)(&recieve_buffers[i]) + 15U) & (~(0x0FU));
        receive_buffer_descriptor_pointer[i].flags = 0xF7FFU | 0x80000000U;
        receive_buffer_descriptor_pointer[i].flags2 = 0U;
        receive_buffer_descriptor_pointer[i].avail = 0U;
    }

    // now we move initialization block into device
    register_address_port.write(1U);
    register_data_port.write((uint32_t)(&initialization_block) & 0xFFFFU);
    register_address_port.write(2U);
    register_data_port.write(((uint32_t)(&initialization_block) >> 16) & 0xFFFFU);
}

AMD_am79c973::~AMD_am79c973() {}

void AMD_am79c973::activate()
{
    register_address_port.write(0U);
    register_data_port.write(0x41U); // enables interrupts

    register_address_port.write(4U);
    uint32_t temp = register_data_port.read();
    register_address_port.write(4U);
    register_data_port.write(temp | 0xC00U);

    register_address_port.write(0U);
    register_data_port.write(0x42U);
}

int AMD_am79c973::reset()
{
    reset_port.read();
    reset_port.write(0);
    return 10;
}

uint32_t AMD_am79c973::handleInterrupt(uint32_t esp)
{
    printf("INTERRUPT from amd am79c973\n");

    register_address_port.write(0);
    uint32_t temp = register_data_port.read();

    if ((temp & 0x8000U) == 0x8000U)
        printf("amd am79c973 GENERAL ERROR\n");
    if ((temp & 0x2000U) == 0x2000U)
        printf("amd am79c973 COLLISION ERROR\n");
    if ((temp & 0x1000U) == 0x1000U)
        printf("amd am79c973 MISSED FRAME\n");
    if ((temp & 0x0800U) == 0x0800U)
        printf("amd am79c973 MEMORY ERROR\n");
    if ((temp & 0x0400U) == 0x0400U)
        receive_data();
    if ((temp & 0x0200U) == 0x0200U)
        printf("amd am79c973 DATA SENT\n");

    // acknowledge
    register_address_port.write(0);
    register_data_port.write(temp);

    if ((temp & 0x0100) == 0x0100)
        printf("amd am79c973 INIT DONE\n");
    return esp;
}

void AMD_am79c973::send_data(common::uint8_t *buffer, int size)
{
    uint8_t send_descriptor_index = current_send_buffer_index;
    current_send_buffer_index = (current_send_buffer_index + 1U) % 8U;

    if(size > 1518) size = 1518; // if to large, just discard

    for(uint8_t *src = buffer + size - 1,
                *dst = (uint8_t*)(send_buffer_descriptor_pointer[send_descriptor_index].address + size - 1);
                src >= buffer; src--, dst--)
    {
        *dst = *src;
    }
    send_buffer_descriptor_pointer[send_descriptor_index].avail = 0; // mark as in use, not allowed to write
    send_buffer_descriptor_pointer[send_descriptor_index].flags2 = 0; // clear error messages
    send_buffer_descriptor_pointer[send_descriptor_index].flags = 0x8300F000 | ((uint16_t)((-size) & 0xFFF));

    // send data with command
    register_address_port.write(0U);
    register_data_port.write(0x48U);
}

void AMD_am79c973::receive_data()
{
    printf("amd am79c973 DATA RECEIVED\n");

    while((receive_buffer_descriptor_pointer[current_receive_buffer_index].flags & 0x80000000U) == 0U)
    {
        if(!(receive_buffer_descriptor_pointer[current_receive_buffer_index].flags & 0x40000000U) // check error bit ERR
         && (receive_buffer_descriptor_pointer[current_receive_buffer_index].flags & 0x03000000U) == 0x03000000U) // check start of packet and end of packet, stp and enp
        {
            uint32_t size = receive_buffer_descriptor_pointer[current_receive_buffer_index].flags & 0xFFFU;
            if(size > 64)
            {
                size -= 4; // remove checksum
            }
            uint8_t *buffer = (uint8_t*)(receive_buffer_descriptor_pointer[current_receive_buffer_index].address);

            for(uint32_t i = 0; i < size ; i++)
            {
                printfHex(buffer[i]);
                printf(" ");
            }
        }

        // now clear the buffer
        receive_buffer_descriptor_pointer[current_receive_buffer_index].flags2 = 0U;
        receive_buffer_descriptor_pointer[current_receive_buffer_index].flags = 0x8000F7FFU;
            
        current_receive_buffer_index = (current_receive_buffer_index + 1) % 8U;
    }
}
