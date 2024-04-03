#ifndef __CPU_H__
#define __CPU_H__

#include<string>
#include<queue>
#include<vector>
#include<string>
#include "process.h"
#include "simout.h"

class arrivalTimeComparator{
    public:
    bool operator()(const Process& p1, const Process& p2) const {
        return p1.getArrivalTime() > p2.getArrivalTime(); // Use '>' for min-heap, '<' for max-heap
    }
};

class waitTimeComparator{
    public:
    bool operator()(const Process& p1, const Process& p2) const{
        return p1.blockedUntil() > p2.blockedUntil();
    }
};
class CPU {
    public:
    CPU(int switchTime) 
    : isRunning(false),
    ctxSwitch(switchTime),
    readyQ(std::priority_queue<Process, std::vector<Process>, std::greater<Process>>()),
    cpuArrivals(std::priority_queue<Process, std::vector<Process>, arrivalTimeComparator>())
    {}
    void addNewProcess(const Process& newProcess) {cpuArrivals.push(newProcess);}
    bool isEmpty();
    void checkProcessArrival(int curTime, simout& statLogger);
    void checkProcessIODone(int curTime, simout& statLogger);
    void checkForReadyProcess(int curTime, simout& statLogger);
    void checkBurstCompleteFR(int curTime, simout& statLogger);
    void checkBurstCompleteSS(int curTime, simout& statLogger);

    void updateRunningProcess(simout& statLogger);
    bool canPreempt();
    bool isIdle() {return !isRunning;}
    bool tSliceExpired();
    int getCTXSwitch() {return ctxSwitch;}
    void preemptProcess(int curTime);
    std::string printQ(std::priority_queue<Process, std::vector<Process>, std::greater<Process>> tmpQ );
    
    static std::string cpuType;
    static int tSlice;
    private:
    bool isRunning;
    int ctxSwitch;
    std::priority_queue<Process, std::vector<Process>, std::greater<Process>> readyQ; 
    std::priority_queue<Process, std::vector<Process>, arrivalTimeComparator> cpuArrivals;
    std::priority_queue<Process, std::vector<Process>, waitTimeComparator> waitingQ;
    Process runningProcess;
    void reportIODone(int curTime, char PID,  int tau);
    void reportProcessArrival(int curTime, char PID,  int tau);
    void reportProcessTerminated(int curTime, char PID);
    void reportRecalculatedTau(int curTime, char PID, int oldTau, int newTau);
    void reportCPUBurstCompleted(int curTime, char PID, int remainingBurst, int tau);
    void reportCPUBurstStart(int curTime, char PID, int burstTime, int tau);
    void reportIOBlockStart(int curTime, char PID, int blockTime);
    void reportCPUBurstContinue(int curTime);
    
};

#endif