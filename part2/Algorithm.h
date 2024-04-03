#ifndef __Algorithm_H__
#define __Algorithm_H__

#include <vector>
#include <queue>
#include "Process.h"
#include "Measurements.h"

struct processComparator
{
    bool operator()(const Process &lhs, const Process &rhs) const
    {
        return lhs.getArrivalTime() > rhs.getArrivalTime();
    }
};

class Algorithm
{
private:
    int time;
    std::string algo;
    std::priority_queue<Process, std::vector<Process>, processComparator> arrival_queue;
    std::priority_queue<Process, std::vector<Process>, std::greater<Process>> ready_queue;
    std::priority_queue<Process, std::vector<Process>, std::greater<Process>> waiting_queue;
    Process current_process;
    int t_cs;
    Measurements measurements_CB; // for CPU-bound processes
    Measurements measurements_IB; // for I/O-bound processes

public:
    Algorithm(std::string algo, int t_cs, std::vector<Process> processes)
        : time(0),
          algo(algo),
          t_cs(t_cs),
          measurements_CB(Measurements()),
          measurements_IB(Measurements())
    {
        for (Process p : processes)
        {
            arrival_queue.push(p);
        }
    }

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
    void incrementMeasurements(Process &p);
};

#endif // __Algorithm_H__