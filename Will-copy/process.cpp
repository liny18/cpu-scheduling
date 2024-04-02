#include "process.h"
#include<iostream>
#include<cmath>
#include "cpu.h"
bool Process::operator>(const Process& rhs) const {
    if(CPU::cpuType == "FCFS" || CPU::cpuType == "RR"){
        return this->FCFS_Comparator(rhs);
    } else if(CPU::cpuType == "SJF" || CPU::cpuType == "SRT" ){
        return this-> SJF_Comparator(rhs);
    } else {
        return false;
    }
}
bool Process::FCFS_Comparator(const Process& other) const {
    if(this->arrivalTime == other.arrivalTime)
    {
        if(this->arrivalStatus == other.arrivalStatus)
            return this->PID > other.PID;
        else
            return this->arrivalStatus < other.arrivalStatus;
    }
    return this->arrivalTime > other.arrivalTime;
}
bool Process::SJF_Comparator(const Process& other) const{
   int time1 = this->tau - this->getTimeElapsed();
    int time2 = other.tau - other.getTimeElapsed();
    if(time1 == time2)
        return this->PID > other.PID;
    return time1 > time2;
}
void Process::recalculateTau(int newBurst) {
    tau = ceil(alpha * newBurst + (1.0 - alpha) * tau);
}

float Process::alpha;