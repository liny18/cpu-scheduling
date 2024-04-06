#include <cstdlib>
#include <cmath>
#include <string>
#include <ctime>
#include <vector>
#include <iostream>
#include <iomanip>
#include "cpu.h"
#include "process.h"
#include "simulations.h"
float next_exp(float Lambda, int upperBound)
{
    float nextExp = 0;
    do
    {
        float r = drand48();
        nextExp = -log(r) / Lambda;
    } while (nextExp > upperBound);

    return nextExp;
}

int main(int argc, char *argv[])
{
    if (argc != 9)
    {
        std::cerr << "Invalid command line arguments" << std::endl;
        return 1;
    }

    // CL Arguments
    int numProcesses = std::stoi(std::string(argv[1]));
    int cpuBound = std::stoi(std::string(argv[2]));
    int seed = std::stoi(std::string(argv[3]));
    float lambda = std::stof(std::string(argv[4]));
    int upperBound = std::stoi(std::string(argv[5]));
    int ctxSwitch = std::stoi(std::string(argv[6]));
    float alpha = std::stof(std::string(argv[7]));
    int tSlice = std::stoi(std::string(argv[8]));
    // Project Initial Setup
    srand48(seed);
    CPU cpu(ctxSwitch);
    Process::alpha = alpha;
    CPU::tSlice = tSlice;

    if (cpuBound > 1)
    {
        std::cout << "<<< PROJECT PART I -- process set (n=" << numProcesses << ") with " << cpuBound << " CPU-bound processes >>>" << std::endl;
    }
    else
    {
        std::cout << "<<< PROJECT PART I -- process set (n=" << numProcesses << ") with " << cpuBound << " CPU-bound process >>>" << std::endl;
    }

    // Populate CPU with Processes
    for (int i = 0; i < numProcesses; ++i)
    {
        char PID = 'A' + i;
        std::string boundType = i >= numProcesses - cpuBound ? "CPU" : "I/O";
        float arrivalTime = floor(next_exp(lambda, upperBound));
        unsigned int numCPUBursts = ceil(drand48() * 64);
        std::cout << boundType << "-bound process " << PID << ": arrival time "
                  << arrivalTime << "ms; " << numCPUBursts << " CPU bursts" << std::endl;
        Process newProcess(PID, boundType, arrivalTime, numCPUBursts, ceil(1 / lambda));
        for (unsigned int j = 0; j < numCPUBursts - 1; ++j)
        {
            int cpuBurst = ceil(next_exp(lambda, upperBound));
            int ioBurst = ceil(next_exp(lambda, upperBound)) * 10;
            if (boundType == "CPU")
            {
                cpuBurst *= 4;
                ioBurst /= 8;
            }
            newProcess.addCPUBurst(cpuBurst);
            newProcess.addIOBurst(ioBurst);
        }
        int finalBurst = boundType == "CPU" ? ceil(next_exp(lambda, upperBound)) * 4 : ceil(next_exp(lambda, upperBound));
        newProcess.addCPUBurst(finalBurst);
        newProcess.setRemainingBurstTime(newProcess.getCPUBurstTime());
        cpu.addNewProcess(newProcess);
    }
    std::cout << std::endl
              << "<<< PROJECT PART II -- t_cs=" << ctxSwitch << "ms; alpha=" << std::fixed << std::setprecision(2) << alpha << "; t_slice=" << tSlice << "ms >>>" << std::endl;
    // FCFS Code
    CPU::cpuType = "FCFS";
    RunFCFS(cpu);
    std::cout << std::endl;

    // //SJF Code
    // CPU::cpuType = "SJF";
    // RunSJF(cpu);
    // std::cout << std::endl;

    // //SRT Code
    // CPU::cpuType = "SRT";
    // RunSRT(cpu);
    // std::cout << std::endl;

    // CPU::cpuType = "RR";
    // RunRR(cpu);

    return 0;
}
