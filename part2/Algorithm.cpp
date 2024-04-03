#include "Algorithm.h"
#include <iostream>

/*

list = [ ]

while pq empty:
    curr = pq.top(), pq.pop
    curr.time--;
    list.append(curr)

for p : list :
    pq.push(p)


while(pq.top().time <= currtime) process



*/

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
    std::cout << "1" << std::endl;
    if (current_process.getBufferTime() == time)
    {
        std::cout << "2" << std::endl;
        if (current_process.getCurrentBurstIndex() == current_process.getCpuBurstCount() - 1)
        {
            std::cout << "3" << std::endl;
            // process has completed all its CPU bursts
            current_process.setStatus("TERMINATED");
        }
        else
        {
            // process has not completed all its CPU bursts
            current_process.setStatus("WAITING");
            std::cout << "o" << std::endl;
            std::cout << current_process.getCurrentBurstIndex() << std::endl;
            current_process.setBufferTime(current_process.getIoBurst());
            std::cout << current_process.getCurrentBurstIndex() << std::endl;
            current_process.setCurrentBurstIndex(current_process.getCurrentBurstIndex() + 1);
            std::cout << "4" << std::endl;
            waiting_queue.push(current_process);
        }
        std::cout << "5" << std::endl;
        incrementMeasurements(current_process);
        std::cout << "6" << std::endl;
    }
    std::cout << "7" << std::endl;
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
    std::cout << "orzbob " << std::endl;
    // check which algo

    while (!isEmpty())
    {
        std::cout << "time " << time << "ms: " << std::endl;
        checkCPUBurstComplete();
        std::cout << "time " << time << "ms: " << std::endl;
        checkStartUsingCPU();
        checkIOBurstComplete();
        checkProcessArrival();
        time++;
    }
}