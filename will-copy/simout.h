#ifndef __SIMOUT_H__
#define __SIMOUT_H__
#include<map>
#include<string>
class stats{
    public:
    stats():
        totalBurstTime(0),
        totalWaitTime(0),
        totalTurnaroundTime(0),
        curBurstStartTime(0),
        ReadyQEntryTime(0),
        numCSwitch(0),
        numBurst(0) {}
    double totalBurstTime;
    double totalWaitTime;
    double totalTurnaroundTime;
    int curBurstStartTime;
    int ReadyQEntryTime;
    int numCSwitch;
    int numPreemptions;
    int numBurst;

    

    std::string type;
};

class simout {
    public:
    simout():
        runningTime(0){}
    void logBurstStart(char PID, int curTime);
    void logBurstComplete(char PID, int curTime, int burstTime);
    void logNewBurst(char PID, const std::string type);
    void logBurstReady(char PID, int curTime);
    void logPreemption(char PID){Processes[PID].numPreemptions += 1;}
    void logCTXIN(char PID, int curTime);
    void logCTXOUT(char PID) { Processes[PID].numCSwitch += 1; }
    void logStats(std::string algo, int time);
    void logRunning(){runningTime += 1;}
    private:
    std::map<char, stats> Processes;
    int runningTime;


};

#endif