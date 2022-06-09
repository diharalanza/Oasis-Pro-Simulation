#include "battery.h"

Battery::Battery()
{

}

double Battery::getBatteryLevel() const
{
    return batteryLevel;
}

void Battery::setBatteryLevel(double newBatteryLevel)
{
    batteryLevel = newBatteryLevel;
}

double Battery::getBatteryConsumeRate() const
{
    return batteryConsumeRate;
}

void Battery::setBatteryConsumeRate(double newBatteryConsumeRate)
{
    batteryConsumeRate = newBatteryConsumeRate;
}

void Battery::consume(bool active, bool paused, int intensity)
{
    if (!active || paused){
        batteryLevel -= batteryConsumeRate;
    }
    else{
        batteryLevel -= batteryConsumeRate * 2 + intensity * 0.005;
    }
    if(batteryLevel < 0)
        batteryLevel = 0;
}
