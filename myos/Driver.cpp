#include "Driver.h"

Driver::Driver() = default;
Driver::~Driver() = default;

void Driver::activate()
{
}

int Driver::reset()
{
    return 0;
}

void Driver::deactivate()
{

}


DriverManager::DriverManager() = default;
DriverManager::~DriverManager() = default;

void DriverManager::add_driver(Driver *driver)
{ 
    if(nof_drivers < 256) drivers[nof_drivers++] = driver;
}

void DriverManager::activate_all()
{
    for(int i = 0; i < nof_drivers; i++)
    {
        drivers[i]->activate();
    }
}
