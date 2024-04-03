#ifndef __Algorithm_H__
#define __Algorithm_H__

#include <vector>
#include <queue>
#include "Process.h"
#include "Measurements.h"

class Algorithm
{
private:
    int time;
    std::string algo;
    std::queue<Process> processes;
    std::priority_queue<Process, std::vector<Process>, std::greater<Process>> ready_queue;
    std::priority_queue<Process, std::vector<Process>, std::greater<Process>> waiting_queue;
    Process current_process;
    int t_cs;
    Measurements measurements_CB; // for CPU-bound processes
    Measurements measurements_IB; // for I/O-bound processes

public:
    Algorithm(std::string algo, int t_cs, std::queue<Process> processes, Measurements measurements_CB, Measurements measurements_IB)
        : algo(algo), t_cs(t_cs), processes(processes), measurements_CB(measurements_CB), measurements_IB(measurements_IB), time(0) {}

    void run();
    void FCFS();
    void RR();
    void SJF();
    void SRT();
    void checkCPUBurstComplete();
    void checkStartUsingCPU();
    void checkIOBurstComplete();
    void checkProcessArrival();
    bool isEmpty();
};

#endif // __Algorithm_H__