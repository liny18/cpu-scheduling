#include "Algorithm.h"

void Algorithm::incrementMeasurements(Process &p)
{
    if (p.is_cpu_bound())
    {
    }
    else
    {
    }
}

bool Algorithm::isEmpty()
{
    return arrival_queue.empty() && ready_queue.empty() && waiting_queue.empty();
}

void Algorithm::checkCPUBurstComplete()
{
    // Process finishes using the CPU
    // Process terminates by finishing its last CPU burst
    // Process starts performing I/O

    // check if the current process has completed its CPU burst, and add it to the waiting queue if it has not completed all its CPU bursts
}

void Algorithm::checkStartUsingCPU()
{
    // Process starts using the CPU

    // check if ready queue has a process that can start using the CPU, set it as the current process
}

void Algorithm::checkIOBurstComplete()
{
    // Process finishes performing I/O
    // Process arrival (at I/O completion)

    // check if processes in the waiting queue have completed their IO burst, and add them to the ready queue
}

void Algorithm::checkProcessArrival()
{
    // Process arrival (initially)

    // check if any process has arrived at the current time, and add it to the ready queue

    while (!arrival_queue.empty() && arrival_queue.top().getArrivalTime() == time)
    {
        Process p = arrival_queue.top();
        arrival_queue.pop();
        ready_queue.push(p);
    }
}

void Algorithm::run()
{
    if (algo == "FCFS")
    {
        FCFS();
    }
    else if (algo == "SJF")
    {
        SJF();
    }
    else if (algo == "SRT")
    {
        SRT();
    }
    else if (algo == "RR")
    {
        RR();
    }

    while (!isEmpty())
    {
        checkCPUBurstComplete();
        checkStartUsingCPU();
        checkIOBurstComplete();
        checkProcessArrival();
        time++;
    }
}