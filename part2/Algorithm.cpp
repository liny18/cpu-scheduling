#include "Algorithm.h"
#include <iostream>

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
    if (current_process.getBufferTime() == time)
    {
        if (current_process.getCurrentBurstIndex() == current_process.getCpuBurstCount() - 1)
        {
            // process has completed all its CPU bursts
            current_process.setStatus("TERMINATED");
        }
        else
        {
            // process has not completed all its CPU bursts
            current_process.setStatus("WAITING");
            current_process.setBufferTime(current_process.getIoBurst());
            current_process.setCurrentBurstIndex(current_process.getCurrentBurstIndex() + 1);
            waiting_queue.push(current_process);
        }
        incrementMeasurements(current_process);
    }
}

void Algorithm::checkStartUsingCPU()
{
    // Process starts using the CPU

    // check if ready queue has a process that can start using the CPU, set it as the current process

    if (current_process.getStatus() == "TERMINATED")
    {
        if (!ready_queue.empty())
        {
            current_process = ready_queue.top();
            current_process.setStatus("RUNNING");
            current_process.setBufferTime(current_process.getCpuBurst());
            ready_queue.pop();
        }
        else
        {
            return;
        }
    }
    else
    {
        return;
    }
}

void Algorithm::checkIOBurstComplete()
{
    // Process finishes performing I/O
    // Process arrival (at I/O completion)

    // check if processes in the waiting queue have completed their IO burst, and add them to the ready queue

    while (!waiting_queue.empty() && waiting_queue.top().getBufferTime() == time)
    {
        Process p = waiting_queue.top();
        waiting_queue.pop();
        p.setStatus("READY");
        ready_queue.push(p);
    }
}

void Algorithm::checkProcessArrival()
{
    // Process arrival (initially)

    // check if any process has arrived at the current time, and add it to the ready queue

    while (!arrival_queue.empty() && arrival_queue.top().getArrivalTime() == time)
    {
        Process p = arrival_queue.top();
        arrival_queue.pop();
        p.setStatus("READY");
        ready_queue.push(p);
    }
}

void Algorithm::run()
{
    // check which algo

    while (!isEmpty())
    {
        checkCPUBurstComplete();
        checkStartUsingCPU();
        checkIOBurstComplete();
        checkProcessArrival();
        time++;
    }
}