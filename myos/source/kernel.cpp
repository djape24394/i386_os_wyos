#include <common/types.h>
#include <GlobalDescriptorTable.h>
#include <hardware_communication/InterruptManager.h>
#include <hardware_communication/pci.h>
#include <drivers/Driver.h>
#include <drivers/KeyboardDriver.h>
#include <drivers/MouseDriver.h>
#include <drivers/VideoGraphicsArray.h>
#include <gui/Desktop.h>
#include <gui/Window.h>
#include <multitasking.h>
#include <memory_management.h>

// #define GRAPHICS_MODE

using namespace myos;
using namespace myos::common;
using namespace myos::hardware_communication;
using namespace myos::drivers;
using namespace myos::gui;

void printf(char *str)
{
    // TODO: refactor this later, no magic numbers
    static uint16_t *VideoMemory = (uint16_t *)0xb8000;
    // y is a row, x is a column, the screen size is 25x80
    static uint8_t x = 0U, y = 0U;
    for (int i = 0; str[i] != '\0'; ++i)
    {
        if (str[i] == '\n')
        {
            x = 0U;
            y++;
        }
        else
        {
            // every other byte is filled, that why we use this uint16_t
            VideoMemory[y * 80 + x] = (VideoMemory[y * 80 + x] & 0xFF00) | str[i];
            x++;
        }

        if (x >= 80)
        {
            y++;
            x = 0U;
        }

        if (y >= 25)
        {
            for (int i = 0; i < 25; i++)
                for (int j = 0; j < 80; j++)
                    VideoMemory[i * 80 + j] = (VideoMemory[i * 80 + j] & 0xFF00) | ' ';
            x = 0U;
            y = 0U;
        }
    }
}

void printfHex(uint8_t key)
{
    char *foo = "0x00";
    char *hex = "0123456789ABCDEF";
    foo[2] = hex[(key >> 4) & 0xF];
    foo[3] = hex[key & 0xF];
    printf(foo);
}

void printfHex16(uint16_t key)
{
    char *foo = "0x0000";
    char *hex = "0123456789ABCDEF";
    foo[2] = hex[(key >> 12) & 0xF];
    foo[3] = hex[(key >> 8) & 0xF];
    foo[4] = hex[(key >> 4) & 0xF];
    foo[5] = hex[key & 0xF];
    printf(foo);
}

void printfHex32(uint32_t key)
{
    char *foo = "0x00000000";
    char *hex = "0123456789ABCDEF";
    foo[2] = hex[(key >> 28) & 0xF];
    foo[3] = hex[(key >> 24) & 0xF];
    foo[4] = hex[(key >> 20) & 0xF];
    foo[5] = hex[(key >> 16) & 0xF];
    foo[6] = hex[(key >> 12) & 0xF];
    foo[7] = hex[(key >> 8) & 0xF];
    foo[8] = hex[(key >> 4) & 0xF];
    foo[9] = hex[key & 0xF];
    printf(foo);
}

void taskA()
{
    while (true)
    {
        printf("A");
    }
}

void taskB()
{
    while (true)
    {
        printf("B");
    }
}

typedef void (*constructor)();
extern "C" constructor start_ctors;
extern "C" constructor end_ctors;
extern "C" void callConstructors()
{
    for (constructor *i = &start_ctors; i != &end_ctors; i++)
    {
        (*i)();
    }
}

class PrintfKeyboardEventHandler : public KeyboardEventHandler
{
public:
    PrintfKeyboardEventHandler() = default;
    void onKeyDown(char c) override
    {
        char *foo = " ";
        foo[0] = c;
        printf(foo);
    }
};

class MouseToConsoleEventHandler : public MouseEventHandler
{
    // set the cursos at the middle of the window (40, 12)
    int8_t x{40};
    int8_t y{12};
    // 25x80 console screen size
    uint16_t *video_memory{(uint16_t *)0xb8000};

public:
    MouseToConsoleEventHandler()
    {
        video_memory[80 * y + x] = ((video_memory[80 * y + x] & 0xF000) >> 4) |
                                   ((video_memory[80 * y + x] & 0x0F00) << 4) |
                                   (video_memory[80 * y + x] & 0x00FF);
    }
    void onActivate() override
    {
    }
    void onMouseDown(uint8_t button) override
    {
        // TODO: click doesn't work
        // video_memory[80 * y + x] = ((video_memory[80 * y + x] & 0xF000) >> 4) |
        //                            ((video_memory[80 * y + x] & 0x0F00) << 4) |
        //                            (video_memory[80 * y + x] & 0x00FF);
    }
    void onMouseUp(uint8_t button) override
    {
        // TODO: click doesn't work
        // video_memory[80 * y + x] = ((video_memory[80 * y + x] & 0xF000) >> 4) |
        //                            ((video_memory[80 * y + x] & 0x0F00) << 4) |
        //                            (video_memory[80 * y + x] & 0x00FF);
    }
    void onMouseMove(int delta_x, int delta_y) override
    {

        // As we said before, every other byte from this address 0xb8000 is character.
        // The first byte represents the color of the foreground and the background(4 + 4 bits, don't know the exact order),
        // doesn't matter. Cursor will switch the color of the foreground and the background
        video_memory[80 * y + x] = ((video_memory[80 * y + x] & 0xF000) >> 4) |
                                   ((video_memory[80 * y + x] & 0x0F00) << 4) |
                                   (video_memory[80 * y + x] & 0x00FF);
        x += delta_x;
        y += delta_y;
        // clip the values of x and y to screen size, we don't want the cursor out of the screen
        if (x < 0)
            x = 0;
        else if (x >= 80)
            x = 79;
        if (y < 0)
            y = 0;
        else if (y >= 25)
            y = 24;
        video_memory[80 * y + x] = ((video_memory[80 * y + x] & 0xF000) >> 4) |
                                   ((video_memory[80 * y + x] & 0x0F00) << 4) |
                                   (video_memory[80 * y + x] & 0x00FF);
    }
};

extern "C" void kernelMain(void *multiboot_structure, uint32_t magicnum)
{
    printf("Hello World!\n");

    GlobalDescriptorTable gdt;

    // Heap address starts at 10 MB. This is hardcoded, option to take pointer after kernel_stack in loader.s, but tricky
    size_t heap_start_address = 10 * 1024 * 1024;
    // ram_size in KB read from multiboot info, can look at gnu muliboot.h
    uint32_t *ram_size = (uint32_t *)(((size_t)multiboot_structure) + 8);
    MemoryManager memory_manager(heap_start_address, *ram_size * 1024 - heap_start_address - 10 * 1024); // 10 * 1024(10 KB padding)

    printf("heap: ");
    printfHex32(heap_start_address); // for 10 MB, should be 0x00A00000, 10 * 2^20 bytes

    void *allocated = memory_manager.malloc(1024);
    printf("\nallocated: ");

    printfHex32((uint32_t)allocated); // shoud be heap address + 16 bytes
    printf("\n");

    TaskManager task_manager;
    // Task task1(&gdt, taskA);
    // Task task2(&gdt, taskB);

    // task_manager.addTask(&task1);
    // task_manager.addTask(&task2);

    InterruptManager interrupt_manager(&gdt, &task_manager);
#ifdef GRAPHICS_MODE
    Desktop desktop(320, 200, 0x00U, 0x00U, 0xA8U);
#endif
    printf("Initializing Hardware, Stage 1\n");
    DriverManager driver_manager;

#ifdef GRAPHICS_MODE
    KeyboardDriver keyboard(&interrupt_manager, &desktop);
#else
    PrintfKeyboardEventHandler keyboard_console_event_handler;
    KeyboardDriver keyboard(&interrupt_manager, &keyboard_console_event_handler);
#endif
    driver_manager.addDriver(&keyboard);

#ifdef GRAPHICS_MODE
    MouseDriver mouse(&interrupt_manager, &desktop);
#else
    MouseToConsoleEventHandler mouse_console_event_handler;
    MouseDriver mouse(&interrupt_manager, &mouse_console_event_handler);
#endif
    driver_manager.addDriver(&mouse);

    PeripheralComponentInterconnectController pci_controller;
    pci_controller.enumerateDrivers(&driver_manager, &interrupt_manager);

    printf("Initializing Hardware, Stage 2\n");
    driver_manager.activateAll();

#ifdef GRAPHICS_MODE
    VideoGraphicsArray vga;

    vga.setMode(320U, 200U, 8U);
    // vga.fillRectangle(0U, 0U, 320U, 200U, 0x00U, 0x00U, 0xA8U);

    Window window1(&desktop, 10, 10, 20, 20, 0xA8U, 0x00U, 0x00U);
    desktop.addChild(&window1);
    Window window2(&desktop, 40, 15, 30, 30, 0x00U, 0xA8U, 0x00U);
    desktop.addChild(&window2);
#endif

    printf("Initializing Hardware, Stage 3\n");
    interrupt_manager.activate();

    // made driver manager drivers array public, just to test if this works
    // AMD_am79c973 *eth0 = (AMD_am79c973*)(driver_manager.drivers[2]);
    // eth0->send_data((uint8_t *)"Hello Network", 13);

    while (1)
    {
#ifdef GRAPHICS_MODE
        desktop.draw(&vga);
#endif
    }
}
