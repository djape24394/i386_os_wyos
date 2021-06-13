#include <drivers/KeyboardDriver.h>
#include <common/print_to_console.h>

using namespace myos::drivers;
using namespace myos::common;
using namespace myos::hardware_communication;

KeyboardEventHandler::KeyboardEventHandler()
{

}

void KeyboardEventHandler::onKeyDown(char)
{

}

void KeyboardEventHandler::onKeyUp(char)
{

}

KeyboardDriver::KeyboardDriver(InterruptManager* interrupt_manager, KeyboardEventHandler* event_handler)
: InterruptHandler(0x21, interrupt_manager),
  data_port(0x60),
  command_port(0x64),
  handler(event_handler)
{
}

void KeyboardDriver::activate()
{
    // when you start operating system, this will just desc
    while (command_port.read() & 0x1)
    {
        data_port.read();
    }
    command_port.write(0xAE); // tells the pic to start sending keyboard interrupts
    command_port.write(0x20); // get current state
    uint8_t status =(data_port.read() | 1) & (~0x10);
    command_port.write(0x60); // set state
    data_port.write(status);
    data_port.write(0xF4); // final keyboard activation
}

uint32_t KeyboardDriver::handleInterrupt(uint32_t esp)
{
    // For each click we get two interrupts, one for press and one for release. 
    // key >= 0x80 is for the release and less is for the press
    
    // When we get the keyboard interrupt, before we send the response to the PIC, we must fetch it, otherwise pic will halt.
    uint8_t key = data_port.read();
    static bool shift_pressed = false;
    if(handler == nullptr)
    {
        return esp;
    }
    switch(key)
    {
        case 0x02: if(!shift_pressed) handler->onKeyDown('1'); else handler->onKeyDown('!'); break;
        case 0x03: if(!shift_pressed) handler->onKeyDown('2'); else handler->onKeyDown('@'); break;
        case 0x04: if(!shift_pressed) handler->onKeyDown('3'); else handler->onKeyDown('#'); break;
        case 0x05: if(!shift_pressed) handler->onKeyDown('4'); else handler->onKeyDown('$'); break;
        case 0x06: if(!shift_pressed) handler->onKeyDown('5'); else handler->onKeyDown('%'); break;
        case 0x07: if(!shift_pressed) handler->onKeyDown('6'); else handler->onKeyDown('^'); break;
        case 0x08: if(!shift_pressed) handler->onKeyDown('7'); else handler->onKeyDown('&'); break;
        case 0x09: if(!shift_pressed) handler->onKeyDown('8'); else handler->onKeyDown('*'); break;
        case 0x0A: if(!shift_pressed) handler->onKeyDown('9'); else handler->onKeyDown('('); break;
        case 0x0B: if(!shift_pressed) handler->onKeyDown('0'); else handler->onKeyDown(')'); break;
        case 0x10: if(!shift_pressed) handler->onKeyDown('q'); else handler->onKeyDown('Q'); break;
        case 0x11: if(!shift_pressed) handler->onKeyDown('w'); else handler->onKeyDown('W'); break;
        case 0x12: if(!shift_pressed) handler->onKeyDown('e'); else handler->onKeyDown('E'); break;
        case 0x13: if(!shift_pressed) handler->onKeyDown('r'); else handler->onKeyDown('R'); break;
        case 0x14: if(!shift_pressed) handler->onKeyDown('t'); else handler->onKeyDown('T'); break;
        case 0x15: if(!shift_pressed) handler->onKeyDown('y'); else handler->onKeyDown('Y'); break;
        case 0x16: if(!shift_pressed) handler->onKeyDown('u'); else handler->onKeyDown('U'); break;
        case 0x17: if(!shift_pressed) handler->onKeyDown('i'); else handler->onKeyDown('I'); break;
        case 0x18: if(!shift_pressed) handler->onKeyDown('o'); else handler->onKeyDown('O'); break;
        case 0x19: if(!shift_pressed) handler->onKeyDown('p'); else handler->onKeyDown('P'); break;
        case 0x1E: if(!shift_pressed) handler->onKeyDown('a'); else handler->onKeyDown('A'); break;
        case 0x1F: if(!shift_pressed) handler->onKeyDown('s'); else handler->onKeyDown('S'); break;
        case 0x20: if(!shift_pressed) handler->onKeyDown('d'); else handler->onKeyDown('D'); break;
        case 0x21: if(!shift_pressed) handler->onKeyDown('f'); else handler->onKeyDown('F'); break;
        case 0x22: if(!shift_pressed) handler->onKeyDown('g'); else handler->onKeyDown('G'); break;
        case 0x23: if(!shift_pressed) handler->onKeyDown('h'); else handler->onKeyDown('H'); break;
        case 0x24: if(!shift_pressed) handler->onKeyDown('j'); else handler->onKeyDown('J'); break;
        case 0x25: if(!shift_pressed) handler->onKeyDown('k'); else handler->onKeyDown('K'); break;
        case 0x26: if(!shift_pressed) handler->onKeyDown('l'); else handler->onKeyDown('L'); break;
        case 0x2C: if(!shift_pressed) handler->onKeyDown('z'); else handler->onKeyDown('Z'); break;
        case 0x2D: if(!shift_pressed) handler->onKeyDown('x'); else handler->onKeyDown('X'); break;
        case 0x2E: if(!shift_pressed) handler->onKeyDown('c'); else handler->onKeyDown('C'); break;
        case 0x2F: if(!shift_pressed) handler->onKeyDown('v'); else handler->onKeyDown('V'); break;
        case 0x30: if(!shift_pressed) handler->onKeyDown('b'); else handler->onKeyDown('B'); break;
        case 0x31: if(!shift_pressed) handler->onKeyDown('n'); else handler->onKeyDown('N'); break;
        case 0x32: if(!shift_pressed) handler->onKeyDown('m'); else handler->onKeyDown('M'); break;
        case 0x33: if(!shift_pressed) handler->onKeyDown(','); else handler->onKeyDown('<'); break;
        case 0x34: if(!shift_pressed) handler->onKeyDown('.'); else handler->onKeyDown('>'); break;
        case 0x35: if(!shift_pressed) handler->onKeyDown('/'); else handler->onKeyDown('?'); break;

        case 0x1C: handler->onKeyDown('\n'); break;
        case 0x39: handler->onKeyDown(' '); break;
        case 0x2A: case 0x36: shift_pressed = true; break;
        case 0xAA: case 0xB6: shift_pressed = false; break;
        default:
        {  
            if(key < 0x80)
            {
                printf("KEYBOARD ");
                printfHex(key);
                break;
            }
        }
    }
    
    return esp;
}

KeyboardDriver::~KeyboardDriver(){}
