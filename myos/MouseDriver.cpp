#include "MouseDriver.h"

void printf(char *);

// Mouse uses the same data port and command port as keyboard
MouseDriver::MouseDriver(InterruptManager *interrupt_manager)
    : InterruptHandler(0x2C, interrupt_manager),
      data_port(0x60),
      command_port(0x64),
      offset(0U),
      buttons(0U)
{
    // set the cursos at the middle of the window (40, 12)
    uint16_t *video_memory = (uint16_t *)0xb8000;
    video_memory[80 * 12 + 40] = ((video_memory[80 * 12 + 40] & 0xF000) >> 4) |
                               ((video_memory[80 * 12 + 40] & 0x0F00) << 4) |
                               (video_memory[80 * 12 + 40] & 0x00FF);

    // when you start operating system, this will just desc
    while (command_port.read() & 0x1)
    {
        data_port.read();
    }
    command_port.write(0xA8); // activate interrupts
    command_port.write(0x20); // get current state
    uint8_t status = data_port.read() | 2;
    command_port.write(0x60); // set state
    data_port.write(status);
    
    command_port.write(0xD4);
    data_port.write(0xF4); // activate mouse
    data_port.read();
}

uint32_t MouseDriver::handleInterrupt(uint32_t esp)
{
    // check command port status, if the sixth bit is not 1, there is no data to read
    uint8_t status = command_port.read();
    if ((status & 0x20) == 0)
    {
        return esp;
    }

    static int8_t x = 40;
    static int8_t y = 12;

    buffer[offset] = data_port.read();
    offset = (offset + 1) % 3;
    if (offset == 0) // now we have all 3 bytes and we can make mova a cursor
    {
        // 25x80 size screen
        static uint16_t *video_memory = (uint16_t *)0xb8000;

        // As we said before, every other byte from this address 0xb8000 is character.
        // The first byte represents the color of the foreground and the background(4 + 4 bits, don't know the exact order),
        // doesn't matter. Cursor will switch the color of the foreground and the background
        video_memory[80 * y + x] = ((video_memory[80 * y + x] & 0xF000) >> 4) |
                                   ((video_memory[80 * y + x] & 0x0F00) << 4) |
                                   (video_memory[80 * y + x] & 0x00FF);
        x += (int8_t)buffer[1];
        y -= (int8_t)buffer[2];
        // clip the values of x and y to screen size, we don't want the cursor out of the screen
        if (x < 0)
            x == 0;
        else if (x >= 80)
            x = 79;
        if (y < 0)
            y == 0;
        else if (y >= 25)
            y = 24;
        video_memory[80 * y + x] = ((video_memory[80 * y + x] & 0xF000) >> 4) |
                                   ((video_memory[80 * y + x] & 0x0F00) << 4) |
                                   (video_memory[80 * y + x] & 0x00FF);
        for(uint8_t i = 0; i < 3; i++)
        {
            if((buffer[0] & (0x01 << i)) != (buttons & (0x01 << i)))
            {
                video_memory[80 * y + x] = ((video_memory[80 * y + x] & 0xF000) >> 4) |
                            ((video_memory[80 * y + x] & 0x0F00) << 4) |
                            (video_memory[80 * y + x] & 0x00FF);
            }
        }
        buttons = buffer[0];
    }   

    return esp;
}

MouseDriver::~MouseDriver() {}
