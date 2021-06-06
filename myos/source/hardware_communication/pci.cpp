#include <hardware_communication/pci.h>
#include <memory_management.h>
#include <drivers/AMD_am79c973.h>
#include <common/print_to_console.h>

using namespace myos;
using namespace myos::common;
using namespace myos::hardware_communication;
using namespace myos::drivers;

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

void PeripheralComponentInterconnectController::enumerateDrivers(drivers::DriverManager *driver_manager, InterruptManager *interrupt_manager)
{
    for (int bus_nmbr = 0; bus_nmbr < 8; bus_nmbr++)
    {
        for (int device_nmbr = 0; device_nmbr < 32; device_nmbr++)
        {
            int nof_functions = (hasDeviceFunctions(bus_nmbr, device_nmbr)) ? 8 : 1;
            for (int function_nmbr = 0; function_nmbr < nof_functions; function_nmbr++)
            {
                auto device_descriptor = getDeviceDescriptor(bus_nmbr, device_nmbr, function_nmbr);
                // vendor_id will be 0 or all 1 if there are no functions
                if ((device_descriptor.vendor_id == 0x0000U) || (device_descriptor.vendor_id == 0xFFFFU))
                {
                    // if we encounter no function we just continue, as it could be possible that there is no
                    // function for function_nmbr == 0, and there is for function_nmbr == 1, etc.
                    continue;
                }

                // for each bus/device/function/ there can be up to 6 bars
                // https://stackoverflow.com/questions/50988614/why-there-are-6-base-address-registers-bars-in-pcie-endpoint
                for (int bar_nmbr = 0; bar_nmbr < 6; bar_nmbr++)
                {
                    BaseAddressRegister bar{getBaseAddressRegister(bus_nmbr, device_nmbr, function_nmbr, bar_nmbr)};
                    if ((bar.address != nullptr) && (bar.type == BaseAddressRegisterType::input_output))
                    {
                        // TODO: see this port_base_number later
                        device_descriptor.port_base_number = (uint32_t)bar.address;
                    }
                }

                // moved this from the for loop above, we want only one driver per device
                // ------------------
                Driver *driver = getDriver(device_descriptor, interrupt_manager);
                if (driver != nullptr)
                    driver_manager->addDriver(driver);
                // ------------------

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

BaseAddressRegister PeripheralComponentInterconnectController::getBaseAddressRegister(uint16_t bus_nmbr, uint16_t device_nmbr, uint16_t function_nmbr, uint16_t bar_nmbr)
{
    BaseAddressRegister bar{};

    uint32_t headertype = read(bus_nmbr, device_nmbr, function_nmbr, 0x0EU) & 0x7FU;
    int maxBars = 6 - (4 * headertype); // in case of 64 bit bars, there are only 2 of them
    if (bar_nmbr >= maxBars)
        return bar;

    uint32_t bar_value = read(bus_nmbr, device_nmbr, function_nmbr, 0x10U + 4U * bar_nmbr); // 0x10U + 4U*bar_nmbr offset to the bar_nmbr bar
    bar.type = (bar_value & 0x01) ? BaseAddressRegisterType::input_output : BaseAddressRegisterType::memory_mapping;
    
    uint32_t temp;

    if(bar.type == BaseAddressRegisterType::memory_mapping)
    {
        switch ((bar_value >> 1) & 0x3U)
        {
        case 0x00U:
            // 32 bit mode
            break;
        case 0x01U:
            // 20 bit mode
            break;
        case 0x10U:
            // 64 bit mode
            break;
        default:
            break;
        }
        bar.prefetchable = (bar_value >> 3) & 0x01U;
    }else
    {
        // input_output
        bar.address = (uint8_t*)(bar_value & (~0x3U));
        bar.prefetchable = false;
    }
    
    
    return bar;
}

Driver *PeripheralComponentInterconnectController::getDriver(PeripheralComponentInterconnectDeviceDescriptor device_descriptor, InterruptManager *interrupt_manager)
{
    Driver *driver{nullptr};
    switch (device_descriptor.vendor_id)
    {
    case 0x1022U: // AMD
        switch (device_descriptor.device_id)
        {
        case 0x2000U: // am79c973
            printf("AMD am79c973\n");
            driver = (Driver*) MemoryManager::active_memory_manager->malloc(sizeof(AMD_am79c973));
            if(driver != nullptr) 
            {
                new (driver) AMD_am79c973(&device_descriptor, interrupt_manager);
                return driver;
            }
            break;
        default:
            break;
        }
        break;
    case 0x8086: // Intel
        
        break;
    default:
        break;
    }

    switch (device_descriptor.class_id)
    {
    case 0x03U: // graphics
        switch (device_descriptor.subclass_id)
        {
        case 0x00: // VGA
            printf("VGA");
            break;
        default:
            break;
        }
        break;
    
    default:
        break;
    }
    return driver;
}
