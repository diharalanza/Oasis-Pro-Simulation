#ifndef TIMER_H
#define TIMER_H
#include <math.h>
class Timer
{
public:
    Timer();
    int get_time_sec();
    int get_time_min();
    void set_time(int input_time);
    void time_flie();
    bool get_start();
    void set_start(bool input_starts);

private:
    bool starts = false;
    int time_min = 20;
    int time_sec = 0;

};
#endif
