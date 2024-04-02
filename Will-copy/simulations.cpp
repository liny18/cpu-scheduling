#include "simulations.h"
#include "cpu.h"
#include "simout.h"

#include<iostream>

void RunFCFS(CPU cpu) {
    simout statLogger;
    int curTime = 0;
    std::cout << "time 0ms: Simulator started for FCFS [Q <empty>]" << std::endl;
    while(true){
        if(cpu.isEmpty())
            break;

        cpu.checkBurstCompleteFR(curTime, statLogger);
        cpu.checkProcessIODone(curTime, statLogger);
        cpu.checkProcessArrival(curTime,statLogger);
        cpu.updateRunningProcess(statLogger);
        curTime += 1;

    }
    statLogger.logStats("FCFS", curTime -1);
    std::cout << "time " << curTime + - 1 << "ms: Simulator ended for FCFS [Q <empty>]" << std::endl;
}

void RunSJF(CPU cpu) {
    simout statLogger;

    int curTime = 0;
    std::cout << "time 0ms: Simulator started for SJF [Q <empty>]" << std::endl;
    while(true){
        if(cpu.isEmpty())
            break;  

        cpu.checkBurstCompleteSS(curTime, statLogger);
        cpu.checkProcessIODone(curTime, statLogger);
        cpu.checkProcessArrival(curTime, statLogger);
        cpu.updateRunningProcess(statLogger);
        curTime += 1;

    }
    statLogger.logStats("SJF", curTime - 1);
    std::cout << "time " << curTime - 1 << "ms: Simulator ended for SJF [Q <empty>]" << std::endl;


}

void RunSRT(CPU cpu) {
    simout statLogger;
    int curTime = 0;
    std::cout << "time 0ms: Simulator started for SRT [Q <empty>]" << std::endl;
    while(true){
        if(cpu.isEmpty())
            break;  
        cpu.checkBurstCompleteSS(curTime, statLogger);
        cpu.checkProcessIODone(curTime, statLogger);
        cpu.checkProcessArrival(curTime, statLogger);
        cpu.updateRunningProcess(statLogger);
        curTime += 1;
    }
    statLogger.logStats("SRT", curTime - 1);
    std::cout << "time " << curTime - 1 << "ms: Simulator ended for SRT [Q <empty>]" << std::endl;   
}

void RunRR(CPU cpu) {   
    simout statLogger;
    int curTime = 0;
    std::cout << "time 0ms: Simulator started for RR [Q <empty>]" << std::endl;
    while(true){
        if(cpu.isEmpty())
            break;
        cpu.checkBurstCompleteFR(curTime, statLogger);
        cpu.checkProcessIODone(curTime, statLogger);
        cpu.checkProcessArrival(curTime, statLogger);
        cpu.updateRunningProcess(statLogger);
        curTime += 1;

    }
    statLogger.logStats("RR", curTime - 1);
    std::cout << "time " << curTime + - 1 << "ms: Simulator ended for RR [Q <empty>]" << std::endl;
}