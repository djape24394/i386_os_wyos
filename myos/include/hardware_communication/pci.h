#ifndef MYOS_HARDWARE_COMMUNICATION_PCI_H
#define MYOS_HARDWARE_COMMUNICATION_PCI_H

#include <common/types.h>
#include <hardware_communication/Port.h>
#include <hardware_communication/InterruptManager.h>
#include <GlobalDescriptorTable.h>
#include <drivers/Driver.h>

namespace myos::hardware_communication
{

    enum class BaseAddressRegisterType
    {
        memory_mapping = 0,
        input_output
    };

    struct BaseAddressRegister
    {
        bool prefetchable{false}; // example: hard drive is prefetchable, keyboard is not
        common::uint8_t *address{nullptr};
        common::uint32_t size{0U};
        BaseAddressRegisterType type{BaseAddressRegisterType::input_output};
    };

    struct PeripheralComponentInterconnectDeviceDescriptor
    {
        common::uint32_t port_base_number;
        common::uint32_t interrupt_number;

        common::uint16_t bus_number;
        common::uint16_t device_number;
        common::uint16_t function_number;

        common::uint16_t vendor_id;
        common::uint16_t device_id;
        common::uint8_t class_id;
        common::uint8_t subclass_id;
        common::uint8_t interface_id;

        common::uint8_t revision_number;
    };

    /**
         * Class for PCI controller communication
         * There are up to 8 busses, each bus can have 32 devices with 8 functions
         * So to refer all the way up to the function, we need 3 + 8 + 3 bits
         * 
         * PCI controller can be used to communicate with variety of devices, therefore we can ask about
         * uint8_t class_id
         * uint8_t subclass_id
         * uint16_t vendor_id
         * uint16_t device_id
         * These fields have some particular memory address offsets, so we also must pass that as an parameter
         */
    class PeripheralComponentInterconnectController
    {
        Port32bit data_port;
        Port32bit command_port;

    public:
        PeripheralComponentInterconnectController();
        ~PeripheralComponentInterconnectController();

        common::uint32_t read(common::uint16_t bus_nmbr, common::uint16_t device_nmbr, common::uint16_t function_nmbr, common::uint32_t register_offset);
        void write(common::uint16_t bus_nmbr, common::uint16_t device_nmbr, common::uint16_t function_nmbr, common::uint32_t register_offset, common::uint32_t value_to_write);
        // We can ask the device if it has some functions, which can speed up enumeration
        bool hasDeviceFunctions(common::uint16_t bus_nmbr, common::uint16_t device_nmbr);
        /**
             * This function will iterate through all possible bus_nmbr, device_nmbr and function_nmbr, then based on
             * uint8_t class_id
             * uint8_t subclass_id
             * uint16_t vendor_id
             * uint16_t device_id
             * will add particular driver for device to device manager
             */
        void enumerateDrivers(drivers::DriverManager *driver_manager, hardware_communication::InterruptManager *interrupt_manager);
        PeripheralComponentInterconnectDeviceDescriptor getDeviceDescriptor(common::uint16_t bus_nmbr, common::uint16_t device_nmbr, common::uint16_t function_nmbr);
        drivers::Driver *getDriver(PeripheralComponentInterconnectDeviceDescriptor device_descriptor, hardware_communication::InterruptManager *interrupt_manager);
        BaseAddressRegister getBaseAddressRegister(common::uint16_t bus_nmbr, common::uint16_t device_nmbr, common::uint16_t function_nmbr, common::uint16_t bar_nmbr);
    };
} // namespace myos::hardware_communication

#endif // MYOS_HARDWARE_COMMUNICATION_PCI_H
