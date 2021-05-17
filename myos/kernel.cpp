#include "types.h"
#include "GlobalDescriptorTable.h"
#include "InterruptManager.h"
#include "KeyboardDriver.h"
#include "MouseDriver.h"

void printf(char* str)
{
    // TODO: refactor this later, no magic numbers
    static uint16_t* VideoMemory = (uint16_t*)0xb8000;
    // y is a row, x is a column, the screen size is 25x80
    static uint8_t x = 0U, y = 0U;
    for(int i = 0; str[i] != '\0'; ++i)
    {
        if(str[i] == '\n')
        {
            x = 0U;
            y++;
        }else
        {
            // every other byte is filled, that why we use this uint16_t
            VideoMemory[y * 80 + x] = (VideoMemory[y * 80 + x] & 0xFF00) | str[i];
            x++;
        }

        if(x >= 80)
        {
            y++;
            x = 0U;
        }

        if(y >= 25)
        {
            for(int i = 0; i < 25; i++)
                for(int j = 0; j < 80; j++) VideoMemory[i * 80 + j] = (VideoMemory[i * 80 + j] & 0xFF00) | ' ';
            x = 0U;
            y = 0U;
        } 
    }
}

typedef void (*constructor)();
extern "C" constructor start_ctors;
extern "C" constructor end_ctors;
extern "C" void callConstructors()
{
    for(constructor* i = &start_ctors; i != &end_ctors; i++)
    {
        (*i)();
    }
}

extern "C" void kernelMain(void* multiboot_structure, uint32_t magicnum)
{
    printf("Hello World!\n");

    GlobalDescriptorTable gdt;
    InterruptManager im(&gdt);

    // before activating the interrupts, we need to instantiate the hardware and activate the hardware
    KeyboardDriver keyboard(&im);
    MouseDriver mouse(&im);

    im.activate();
    printf("Interrupts Activated!\n");

    while(1);
}
