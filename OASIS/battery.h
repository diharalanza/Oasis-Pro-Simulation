#ifndef BATTERY_H
#define BATTERY_H

#include <QObject>

// the capacity of the battery, change it to a large value when in actual use
#define MAX_BATTERY_CAPACITY 8
class Battery
{
public:
    Battery();
    double getBatteryLevel() const;
    void setBatteryLevel(double newBatteryLevel);
    double getBatteryConsumeRate() const;
    void setBatteryConsumeRate(double newBatteryConsumeRate);

    // consume the battery once according to the batteryConsumeRate
    void consume(bool, bool, int);

private:
    double batteryLevel = MAX_BATTERY_CAPACITY;
    double batteryConsumeRate = 0.002;
};

#endif // BATTERY_H
