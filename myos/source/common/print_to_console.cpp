#include <common/print_to_console.h>
#include <common/types.h>

using namespace myos;

namespace myos::common
{
    void printf(const char *str)
    {
        // For printing to the console, print starting from address 0xb8000(platform specific)
        // For each caracter, 2 bytes are used, first byte represents the foreground and background color, second byte is the char.
        // No need to set the color byte, as it is black background, white foreground by default.
        // y is a row, x is a column, the screen size is 25x80
        static uint16_t *VideoMemory = (uint16_t *)0xb8000;
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
        char foo[5] = "0x00";
        char hex[17] = "0123456789ABCDEF";
        foo[2] = hex[(key >> 4) & 0xF];
        foo[3] = hex[key & 0xF];
        printf(foo);
    }

    void printfHex16(uint16_t key)
    {
        char foo[7] = "0x0000";
        char hex[17] = "0123456789ABCDEF";
        foo[2] = hex[(key >> 12) & 0xF];
        foo[3] = hex[(key >> 8) & 0xF];
        foo[4] = hex[(key >> 4) & 0xF];
        foo[5] = hex[key & 0xF];
        printf(foo);
    }

    void printfHex32(uint32_t key)
    {
        char foo[11] = "0x00000000";
        char hex[17] = "0123456789ABCDEF";
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
} // namespace myos::common