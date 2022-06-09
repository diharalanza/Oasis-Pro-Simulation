#include "timer.h"

Timer::Timer()
{

}

int Timer::get_time_sec(){
    return time_sec;
}
int Timer::get_time_min(){
    return time_min;
}
void Timer::set_time(int input_time){
    time_min = input_time/60;
    time_sec = input_time%60;
}
void Timer::time_flie(){
    if(time_min == 0 && time_sec == 0){
        return;
    }
    if(time_sec==0){
        time_min -=1;
        time_sec =60;
    }
    time_sec -= 1;
}
bool Timer::get_start(){
    return starts;
}
void Timer::set_start(bool input_starts){
    starts = input_starts;
}
