#ifndef MYOS_HARDWARE_COMMUNICATION_PORT_H
#define MYOS_HARDWARE_COMMUNICATION_PORT_H

#include <common/types.h>

namespace myos
{
    namespace hardware_communication
    {

        /*
 * Class Port for communication with the hardware 
 */

        // TODO: make those classes templates, becase 8, 16, 32 bit need outb outw outl, these would be specializations
        class Port
        {
        protected:
            myos::common::uint16_t port_number;
            Port(myos::common::uint16_t port_number);
        };

        class Port8bit : public Port
        {
        public:
            Port8bit(myos::common::uint16_t port_number);
            virtual void write(myos::common::uint8_t data);
            virtual myos::common::uint8_t read();
        };

        class Port8bitSlow : public Port8bit
        {
        public:
            Port8bitSlow(myos::common::uint16_t port_number);
            virtual void write(myos::common::uint8_t data) override;
        };

        class Port16bit : public Port
        {
        public:
            Port16bit(myos::common::uint16_t port_number);
            virtual void write(myos::common::uint16_t data);
            virtual myos::common::uint16_t read();
        };

        class Port32bit : public Port
        {
        public:
            Port32bit(myos::common::uint16_t port_number);
            virtual void write(myos::common::uint32_t data);
            virtual myos::common::uint32_t read();
        };
    } // namespace hardware_communication
} // namespace myos

#endif // MYOS_HARDWARE_COMMUNICATION_PORT_H
