#include <hardware_communication/pci.h>

using namespace myos::common;
using namespace myos::hardware_communication;
using namespace myos::drivers;

void printf(char *);
void printfHex(uint8_t key);
void printfHex16(uint16_t key);

PeripheralComponentInterconnectController::PeripheralComponentInterconnectController()
: data_port(0xCFC),
  command_port(0xCF8)
{
}

PeripheralComponentInterconnectController::~PeripheralComponentInterconnectController()
{
}

uint32_t PeripheralComponentInterconnectController::read(uint16_t bus_nmbr, uint16_t device_nmbr, uint16_t function_nmbr, uint32_t register_offset)
{
    // for register offset, we can only addres with multiple of 4, so last 2 bits are always zero, later we extract what we need
    uint32_t id = (0x1 << 31) |
                  ((bus_nmbr & 0xFF) << 16) |
                  ((device_nmbr & 0x1F) << 11) |
                  ((function_nmbr & 0x07) << 8) |
                  (register_offset & 0xFC);
    command_port.write(id);
    uint32_t result{data_port.read()};
    return result >> (8 * (register_offset % 4)); 
}
void PeripheralComponentInterconnectController::write(uint16_t bus_nmbr, uint16_t device_nmbr, uint16_t function_nmbr, uint32_t register_offset, uint32_t value_to_write)
{
    uint32_t id = (0x1 << 31) |
                  ((bus_nmbr & 0xFF) << 16) |
                  ((device_nmbr & 0x1F) << 11) |
                  ((function_nmbr & 0x07) << 8) |
                  (register_offset & 0xFC);
    command_port.write(id);
    data_port.write(value_to_write);
}

bool PeripheralComponentInterconnectController::hasDeviceFunctions(uint16_t bus_nmbr, uint16_t device_nmbr)
{
    return read(bus_nmbr, device_nmbr, 0U, 0x0E) & (1 << 7);
}

void PeripheralComponentInterconnectController::enumerateDrivers(drivers::DriverManager *driver_manager)
{
    for(int bus_nmbr = 0; bus_nmbr < 8; bus_nmbr++)
    {
        for(int device_nmbr = 0; device_nmbr < 32; device_nmbr++)
        {
            int nof_functions = (hasDeviceFunctions(bus_nmbr, device_nmbr)) ? 8 : 1;
            for(int function_nmbr = 0; function_nmbr < nof_functions; function_nmbr++)
            {
                auto device_descriptor = getDeviceDescriptor(bus_nmbr, device_nmbr, function_nmbr);
                // vendor_id will be 0 or all 1 if there are no functions
                if((device_descriptor.vendor_id == 0x0000U) || (device_descriptor.vendor_id == 0xFFFFU))
                {
                    // if we encounter no function we just continue, as it could be possible that there is no 
                    // function for function_nmbr == 0, and there is for function_nmbr == 1, etc.
                    continue;
                }
                printf("PCI BUS ");
                printfHex(bus_nmbr & 0xFF);

                printf(", DEVICE ");
                printfHex(device_nmbr & 0xFF);

                printf(", FUNCTION ");
                printfHex(function_nmbr & 0xFF);

                printf(" : VENDOR ");
                printfHex16(device_descriptor.vendor_id);

                printf(", DEVICE ");
                printfHex16(device_descriptor.device_id);
                printf("\n");
            }
        }
    }
}

PeripheralComponentInterconnectDeviceDescriptor PeripheralComponentInterconnectController::getDeviceDescriptor(uint16_t bus_nmbr, uint16_t device_nmbr, uint16_t function_nmbr)
{
    PeripheralComponentInterconnectDeviceDescriptor result;
    
    result.bus_number = bus_nmbr;
    result.device_number = device_nmbr;
    result.function_number = function_nmbr;

    result.vendor_id = read(bus_nmbr, device_nmbr, function_nmbr, 0x00U);
    result.device_id = read(bus_nmbr, device_nmbr, function_nmbr, 0x02U);
    
    result.class_id = read(bus_nmbr, device_nmbr, function_nmbr, 0x0BU);
    result.subclass_id = read(bus_nmbr, device_nmbr, function_nmbr, 0x0AU);
    result.interface_id = read(bus_nmbr, device_nmbr, function_nmbr, 0x09U);

    result.revision_number = read(bus_nmbr, device_nmbr, function_nmbr, 0x08U);
    result.interrupt_number = read(bus_nmbr, device_nmbr, function_nmbr, 0x3CU);

    return result;
}
