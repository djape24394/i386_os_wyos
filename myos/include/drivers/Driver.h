#ifndef MYOS_DRIVERS_DRIVER_H
#define MYOS_DRIVERS_DRIVER_H

#include <common/types.h>

namespace myos
{
    namespace drivers
    {
        class Driver
        {
        public:
            Driver();
            ~Driver();
            virtual void activate();
            virtual int reset();
            virtual void deactivate();
        };

        class DriverManager
        {
        private:
            Driver *drivers[256];
            myos::common::int32_t nof_drivers{0};

        public:
            DriverManager();
            ~DriverManager();
            void addDriver(Driver *driver);
            void activateAll();
        };
    } // namespace drivers
} // namespace myos

#endif // MYOS_DRIVERS_DRIVER_H
