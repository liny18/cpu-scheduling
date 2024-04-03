#ifndef __Algorithm_H__
#define __Algorithm_H__

#include <vector>
#include <string>
#include <queue>
#include "Process.h"
#include "Measurements.h"
#include <iostream>
struct arrivalComparator
{
    bool operator()(const Process &lhs, const Process &rhs) const
    {
        return lhs.getArrivalTime() > rhs.getArrivalTime();
    }
};

struct waitingComparator
{
    bool operator()(const Process &lhs, const Process &rhs) const
    {
        return lhs.getBufferTime() > rhs.getBufferTime();
    }
};

struct readyComparator
{
    std::string input_algorithm;

    readyComparator(const std::string &algo) : input_algorithm(algo) {}

    bool operator()(const Process &lhs, const Process &rhs) const
    {
        if (input_algorithm == "FCFS" || input_algorithm == "RR")
        {
            if (lhs.getArrivalTime() == rhs.getArrivalTime())
            {
                return lhs.getId() > rhs.getId();
            }
            return lhs.getArrivalTime() > rhs.getArrivalTime();
        }
        else
        {
            return false;
        }
    }
};

class Algorithm
{
private:
    int time;
    std::string algo;
    std::priority_queue<Process, std::vector<Process>, arrivalComparator> arrival_queue;
    std::priority_queue<Process, std::vector<Process>, readyComparator> ready_queue;
    std::priority_queue<Process, std::vector<Process>, waitingComparator> waiting_queue;
    Process current_process;
    int t_cs;
    Measurements measurements_CB; // for CPU-bound processes
    Measurements measurements_IB; // for I/O-bound processes

public:
    Algorithm(const std::string &input_algo, int t_cs, std::vector<Process> processes)
        : time(0),
          algo(input_algo),
          t_cs(t_cs),
          measurements_CB(Measurements()),
          measurements_IB(Measurements()),
          ready_queue(readyComparator(input_algo))
    {
        for (Process p : processes)
        {
            arrival_queue.push(p);
        }
        current_process = arrival_queue.top();
        arrival_queue.pop();
    }

    void run();
    void checkCPUBurstComplete();
    void checkStartUsingCPU();
    void checkIOBurstComplete();
    void checkProcessArrival();
    bool isEmpty();
    void incrementMeasurements(Process &p);

    std::string getAlgo() const { return algo; }
};

#endif // __Algorithm_H__