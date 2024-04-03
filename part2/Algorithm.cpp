#include "Algorithm.h"

bool Algorithm::isEmpty()
{
    return processes.empty() && ready_queue.empty() && waiting_queue.empty();
}

void Algorithm::checkCPUBurstComplete()
{
    // check if the current process has completed its CPU burst, and add it to the waiting queue if it has not completed all its CPU bursts
}

void Algorithm::checkStartUsingCPU()
{
    // check if ready queue has a process that can start using the CPU, set it as the current process
}

void Algorithm::checkIOBurstComplete()
{
    // check if processes in the waiting queue have completed their IO burst, and add them to the ready queue
}

void Algorithm::checkProcessArrival()
{
    // check if any process has arrived at the current time, and add it to the ready queue
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