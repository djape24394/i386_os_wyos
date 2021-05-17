#include "KeyboardDriver.h"

void printf(char *);

KeyboardDriver::KeyboardDriver(InterruptManager* interrupt_manager)
: InterruptHandler(0x21, interrupt_manager),
  data_port(0x60),
  command_port(0x64)
{
    // when you start operating system, this will just desc
    while (command_port.read() & 0x1)
    {
        data_port.read();
    }
    command_port.write(0xAE); // tells something to keyboard
    command_port.write(0x20); // get current state
    uint8_t status =(data_port.read() | 1) & (~0x10);
    command_port.write(0x60); // set state
    data_port.write(status);
    data_port.write(0xF4); // activate keyboard
}

uint32_t KeyboardDriver::handleInterrupt(uint32_t esp)
{
    // For each click we get two interrupts, one for press and one for release. 
    // key >= 0x80 is for the release and below for the press
    uint8_t key = data_port.read();
    static bool shift_pressed = false;

    switch(key)
    {
        case 0x02: if(!shift_pressed) printf("1"); else printf("!"); break;
        case 0x03: if(!shift_pressed) printf("2"); else printf("@"); break;
        case 0x04: if(!shift_pressed) printf("3"); else printf("#"); break;
        case 0x05: if(!shift_pressed) printf("4"); else printf("$"); break;
        case 0x06: if(!shift_pressed) printf("5"); else printf("%"); break;
        case 0x07: if(!shift_pressed) printf("6"); else printf("^"); break;
        case 0x08: if(!shift_pressed) printf("7"); else printf("&"); break;
        case 0x09: if(!shift_pressed) printf("8"); else printf("*"); break;
        case 0x0A: if(!shift_pressed) printf("9"); else printf("("); break;
        case 0x0B: if(!shift_pressed) printf("0"); else printf(")"); break;
        case 0x10: if(!shift_pressed) printf("q"); else printf("Q"); break;
        case 0x11: if(!shift_pressed) printf("w"); else printf("W"); break;
        case 0x12: if(!shift_pressed) printf("e"); else printf("E"); break;
        case 0x13: if(!shift_pressed) printf("r"); else printf("R"); break;
        case 0x14: if(!shift_pressed) printf("t"); else printf("T"); break;
        case 0x15: if(!shift_pressed) printf("y"); else printf("Y"); break;
        case 0x16: if(!shift_pressed) printf("u"); else printf("U"); break;
        case 0x17: if(!shift_pressed) printf("i"); else printf("I"); break;
        case 0x18: if(!shift_pressed) printf("o"); else printf("O"); break;
        case 0x19: if(!shift_pressed) printf("p"); else printf("P"); break;
        case 0x1E: if(!shift_pressed) printf("a"); else printf("A"); break;
        case 0x1F: if(!shift_pressed) printf("s"); else printf("S"); break;
        case 0x20: if(!shift_pressed) printf("d"); else printf("D"); break;
        case 0x21: if(!shift_pressed) printf("f"); else printf("F"); break;
        case 0x22: if(!shift_pressed) printf("g"); else printf("G"); break;
        case 0x23: if(!shift_pressed) printf("h"); else printf("H"); break;
        case 0x24: if(!shift_pressed) printf("j"); else printf("J"); break;
        case 0x25: if(!shift_pressed) printf("k"); else printf("K"); break;
        case 0x26: if(!shift_pressed) printf("l"); else printf("L"); break;
        case 0x2C: if(!shift_pressed) printf("z"); else printf("Z"); break;
        case 0x2D: if(!shift_pressed) printf("x"); else printf("X"); break;
        case 0x2E: if(!shift_pressed) printf("c"); else printf("C"); break;
        case 0x2F: if(!shift_pressed) printf("v"); else printf("V"); break;
        case 0x30: if(!shift_pressed) printf("b"); else printf("B"); break;
        case 0x31: if(!shift_pressed) printf("n"); else printf("N"); break;
        case 0x32: if(!shift_pressed) printf("m"); else printf("M"); break;
        case 0x33: if(!shift_pressed) printf(","); else printf("<"); break;
        case 0x34: if(!shift_pressed) printf("."); else printf(">"); break;
        case 0x35: if(!shift_pressed) printf("/"); else printf("?"); break;

        case 0x1C: printf("\n"); break;
        case 0x39: printf(" "); break;
        case 0x2A: case 0x36: shift_pressed = true; break;
        case 0xAA: case 0xB6: shift_pressed = false; break;
        default:
        {
            if(key < 0x80)
            {
                char* foo = "KEYBOARD 0x00 ";
                char* hex = "0123456789ABCDEF";
                foo[11] = hex[(key >> 4) & 0xF];
                foo[12] = hex[key & 0xF];
                printf(foo);
                break;
            }
        }
    }
    
    return esp;
}

KeyboardDriver::~KeyboardDriver(){}
