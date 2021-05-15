#ifndef PORT_H
#define PORT_H

#include "types.h"

/*
 * Class Port for communication with the hardware 
 */

// TODO: make those classes templates, becase 8, 16, 32 bit need outb outw outl, these would be specializations
class Port
{
protected:
    uint16_t port_number;
    Port(uint16_t port_number);
};

class Port8bit: public Port
{
public:
    Port8bit(uint16_t port_number);
    virtual void write(uint8_t data);
    virtual uint8_t read();
};

class Port8bitSlow: public Port8bit
{
public:
    Port8bitSlow(uint16_t port_number);
    virtual void write(uint8_t data) override;
};

class Port16bit: public Port
{
public:
    Port16bit(uint16_t port_number);
    virtual void write(uint16_t data);
    virtual uint16_t read();
};

class Port32bit: public Port
{
public:
    Port32bit(uint16_t port_number);
    virtual void write(uint32_t data);
    virtual uint32_t read();
};

#endif
