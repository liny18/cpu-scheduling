#include "simout.h"
#include<map>
#include<fstream>
#include<iostream>
#include<string>
#include<cmath>
#include<iomanip>
#include<algorithm>

 void simout::logBurstStart(char PID, int curTime){
        Processes[PID].curBurstStartTime = curTime ;
       // std::cout  << PID << " start " << Processes[PID].curBurstStartTime << std::endl;
}
void simout::logCTXIN(char PID, int curTime){
    Processes[PID].numCSwitch += 1;        
    Processes[PID].totalWaitTime += curTime - Processes[PID].ReadyQEntryTime;
   // std::cout << PID << " " << curTime - Processes[PID].ReadyQEntryTime << std::endl;
    

}

void simout::logBurstComplete(char PID, int curTime, int burstTime){
    Processes[PID].totalBurstTime += burstTime;
    Processes[PID].totalTurnaroundTime += curTime - Processes[PID].curBurstStartTime;
    ++Processes[PID].numBurst;
    //std::cout << PID << " " << curTime - Processes[PID].curBurstStartTime << std::endl;
}
void simout::logNewBurst(char PID, const std::string type){
    Processes[PID] = stats();
    Processes[PID].type = type;
}
void simout::logBurstReady(char PID, int curTime){
    Processes[PID].ReadyQEntryTime = curTime;
}
void simout::logStats(std::string algo, int time){
    int totalBurstTime = 0;
    int CPUBurstTime= 0;
    int IOBurstTime= 0;
    int totalWaitTime= 0;
    int CPUWaitTime= 0;
    int IOWaitTime= 0;
    int totalTurnaround= 0;
    int CPUTurnaround= 0;
    int IOTurnaround= 0;
    int totalCTX= 0;
    int CPUCTX= 0;
    int IOCTX= 0;
    int totalPreemptions= 0;
    int CPUPreemptions= 0;
    int IOPreemptions= 0;
    int CPUBound = 0;
    int IOBound = 0;
    int totalBursts = 0;
    for(std::map<char,stats>::iterator i = Processes.begin(); i != Processes.end(); ++i){
        //std::cout << i->first << i->second.totalWaitTime << std::endl;
        totalBurstTime += i->second.totalBurstTime;
        totalWaitTime += i->second.totalWaitTime;
        totalTurnaround += i->second.totalTurnaroundTime;
        totalCTX += i->second.numCSwitch;
        totalPreemptions += i->second.numPreemptions;
        totalBursts += i->second.numBurst;
        if(i->second.type == "CPU"){
            CPUBurstTime += i->second.totalBurstTime;
            CPUWaitTime += i->second.totalWaitTime;
            CPUTurnaround += i->second.totalTurnaroundTime;
            CPUCTX += i->second.numCSwitch;
            CPUPreemptions += i->second.numPreemptions;
            CPUBound += i->second.numBurst;

        } else {
            IOBurstTime += i->second.totalBurstTime;
            IOWaitTime += i->second.totalWaitTime;
            IOTurnaround += i->second.totalTurnaroundTime;
            IOCTX += i->second.numCSwitch;
            IOPreemptions += i->second.numPreemptions;
            IOBound += i->second.numBurst;
        }
    }
  //  std::cout <<totalBurstTime << " " << totalBursts<< std::endl;
    double avgCPUBurstTime = CPUBound != 0 ? ceil(static_cast<double>(CPUBurstTime) / CPUBound * 1000.0) / 1000.0 : 0.0;
    double avgIOBurstTime = IOBound != 0 ? ceil(static_cast<double>(IOBurstTime) / IOBound * 1000.0) / 1000.0 : 0.0;
    double avgCPUWaitTime = CPUBound != 0 ? ceil(static_cast<double>(CPUWaitTime) / CPUBound * 1000.0) / 1000.0 : 0.0;
    double avgIOWaitTime = IOBound != 0 ? ceil(static_cast<double>(IOWaitTime) / IOBound * 1000.0) / 1000.0 : 0.0;
    double avgCPUTurnaround = CPUBound != 0 ? ceil(static_cast<double>(CPUTurnaround) / CPUBound * 1000.0) / 1000.0 : 0.0;
    double avgIOTurnaround = IOBound != 0 ? ceil(static_cast<double>(IOTurnaround) / IOBound * 1000.0) / 1000.0 : 0.0;

    double avgTotBurstTime = totalBursts != 0 ? ceil(static_cast<double>(totalBurstTime) / totalBursts * 1000.0) / 1000.0 : 0.0;
    double avgTotWaitTime = totalBursts != 0 ? ceil(static_cast<double>(totalWaitTime) / totalBursts * 1000.0) / 1000.0 : 0.0;
    double avgTurnaround = totalBursts != 0 ? ceil(static_cast<double>(totalTurnaround) / totalBursts * 1000.0) / 1000.0 : 0.0;


     std::ofstream outFile("simout.txt",std::fstream::in | std::fstream::out | std::fstream::app);
    if (outFile.is_open()) {
        outFile << "Algorithm "<< algo << std::endl;
        outFile << "-- CPU utilization: " << std::fixed << std::setprecision(3) << ceil((static_cast<double>(runningTime) / time) * 100.0 * 1000 ) / 1000.0  << "%" << std::endl;
        outFile << "-- average CPU burst time: " << avgTotBurstTime << " ms (" << avgCPUBurstTime << " ms/" << avgIOBurstTime << " ms)" << std::endl;
        outFile << "-- average wait time: " << avgTotWaitTime << " ms (" << avgCPUWaitTime << " ms/" << avgIOWaitTime << " ms)" << std::endl;
        outFile << "-- average turnaround time: " << avgTurnaround << " ms (" << avgCPUTurnaround << " ms/" << avgIOTurnaround << " ms)" << std::endl;
        outFile << "-- number of context switches: " << totalCTX / 2 << " (" << CPUCTX / 2<< "/" << IOCTX / 2 << ")" << std::endl;
        outFile << "-- number of preemptions: " << totalPreemptions << " (" << CPUPreemptions << "/" << IOPreemptions << ")" << std::endl;
        if(algo != "RR")
            outFile << std::endl;
        outFile.close();
    } else {
        std::cerr << "Error opening simout.txt for writing." << std::endl;
    }

}