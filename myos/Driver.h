#ifndef DRIVER_H
#define DRIVER_H

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
    Driver* drivers[256];
    int nof_drivers{0};
public:
    DriverManager();
    ~DriverManager();
    void add_driver(Driver *driver);
    void activate_all();
};

#endif // DRIVER