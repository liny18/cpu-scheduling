#include "cpu.h"
#include <iostream>
#include <string>
#include <assert.h>

// static variable initialization
std::string CPU::cpuType;
int CPU::tSlice;
// CPU Operation Print Functions
std::string CPU::printQ(std::priority_queue<Process, std::vector<Process>, std::greater<Process>> tmpQ)
{
    std::string result = "[Q ";
    /* if(isRunning && runningProcess.getStatus() == "CTX_IN"){
         result += std::string(1,runningProcess.getPID() ) + " ";
     }
     else*/
    if (tmpQ.empty())
    {
        return "[Q <empty>]";
    }
    while (!tmpQ.empty())
    {
        result += std::string(1, tmpQ.top().getPID()) + " ";
        // std::cout << "Printing "<< tmpQ.top().getPID() << ": " << tmpQ.top().getArrivalTime() << " " << tmpQ.top().getArrivalStatus() << std::endl;
        tmpQ.pop();
    }
    result[result.size() - 1] = ']';
    return result;
}

void CPU::reportIODone(int curTime, char PID, int tau)
{
    if (curTime > 9999)
        return;

    if (CPU::cpuType == "SJF" || CPU::cpuType == "SRT")
    {
        std::cout << "time " << curTime << "ms: Process " << PID << " (tau " << tau << "ms) completed I/O;";
    }
    else
    {
        std::cout << "time " << curTime << "ms: Process " << PID << " completed I/O;";
    }
}

void CPU::reportProcessArrival(int curTime, char PID, int tau)
{
    if (curTime > 9999)
        return;
    if (CPU::cpuType == "SJF" || CPU::cpuType == "SRT")
    {
        std::cout << "time " << curTime << "ms: Process " << PID << " (tau " << tau << "ms) arrived;";
    }
    else
    {
        std::cout << "time " << curTime << "ms: Process "
                  << PID << " arrived;";
    }
}
void CPU::reportProcessTerminated(int curTime, char PID)
{

    std::cout << "time " << curTime << "ms: Process "
              << PID << " terminated "
              << this->printQ(readyQ) << std::endl;
}
void CPU::reportRecalculatedTau(int curTime, char PID, int oldTau, int newTau)
{
    if (curTime > 9999)
        return;
    std::cout << "time " << curTime
              << "ms: Recalculating tau for process " << PID << ": old tau " << oldTau
              << "ms ==> new tau " << newTau << "ms " << this->printQ(readyQ) << std::endl;
}
void CPU::reportCPUBurstCompleted(int curTime, char PID, int remainingBurst, int tau)
{
    if (curTime > 9999)
        return;
    if (CPU::cpuType == "SJF" || CPU::cpuType == "SRT")
    {
        if (remainingBurst > 1)
        {
            std::cout << "time " << curTime << "ms: Process " << PID << " (tau " << tau << "ms) completed a CPU burst; "
                      << remainingBurst << " bursts to go " << this->printQ(readyQ) << std::endl;
        }
        else
        {
            std::cout << "time " << curTime << "ms: Process " << PID << " (tau " << tau << "ms) completed a CPU burst; "
                      << "1 burst to go " << this->printQ(readyQ) << std::endl;
        }
    }
    else
    {
        if (remainingBurst > 1)
        {
            std::cout << "time " << curTime << "ms: Process " << PID << " completed a CPU burst; "
                      << remainingBurst << " bursts to go " << this->printQ(readyQ) << std::endl;
        }
        else
        {
            std::cout << "time " << curTime << "ms: Process " << PID << " completed a CPU burst; "
                      << "1 burst to go " << this->printQ(readyQ) << std::endl;
        }
    }
}
void CPU::reportCPUBurstStart(int curTime, char PID, int burstTime, int tau)
{
    if (curTime > 9999)
        return;
    if (CPU::cpuType == "SJF" || CPU::cpuType == "SRT")
    {
        std::cout << "time " << curTime << "ms: Process " << PID
                  << " (tau " << tau << "ms) started using the CPU for " << burstTime << "ms burst " << this->printQ(readyQ) << std::endl;
    }
    else
    {
        std::cout << "time " << curTime << "ms: Process " << PID << " started using the CPU for " << burstTime << "ms burst " << this->printQ(readyQ) << std::endl;
    }
}

void CPU::reportIOBlockStart(int curTime, char PID, int blockTime)
{
    if (curTime > 9999)
        return;
    std::cout << "time " << curTime << "ms: Process "
              << PID << " switching out of CPU; blocking on I/O until time " << blockTime
              << "ms " << this->printQ(readyQ) << std::endl;
}

void CPU::reportCPUBurstContinue(int curTime)
{
    if (curTime > 9999)
        return;
    if (CPU::cpuType == "SRT")
    {
        std::cout << "time " << curTime << "ms: Process " << runningProcess.getPID() << " (tau " << runningProcess.getTau() << "ms) started using the CPU for remaining "
                  << runningProcess.getRemainingBurstTime() << "ms of " << runningProcess.getCPUBurstTime() << "ms burst " << this->printQ(readyQ) << std::endl;
    }
    else
    {
        std::cout << "time " << curTime << "ms: Process " << runningProcess.getPID() << " started using the CPU for remaining "
                  << runningProcess.getRemainingBurstTime() << "ms of " << runningProcess.getCPUBurstTime() << "ms burst " << this->printQ(readyQ) << std::endl;
    }
}

// CPU Process Logic
bool CPU::isEmpty()
{
    return readyQ.size() == 0 && cpuArrivals.size() == 0 && !isRunning && waitingQ.size() == 0;
}
bool CPU::canPreempt()
{
    if (CPU::cpuType == "FCFS" || CPU::cpuType == "SJF" || !(isRunning && runningProcess.getStatus() == "RUNNING") || readyQ.size() == 0)
    {
        return false;
    }
    else
    {
        if (CPU::cpuType == "SRT")
        {
            if (runningProcess.getTau() - runningProcess.getTimeElapsed() > readyQ.top().getTau() - readyQ.top().getTimeElapsed())
            {
                // std::cout << "Reason for Preempt Running Process Time: " << runningProcess.getTau()  << "Preempting process: " << readyQ.top().getTau() - readyQ.top().getTimeElapsed() << std::endl;
                // std::cout << readyQ.top().getCPUBurstTime() << " " << readyQ.top().getRemainingBurstTime();
                return true;
            }

            else
            {
                // std::cout << "Reason for No Preempt Running Process Time: " << runningProcess.getTau() - runningProcess.getTimeElapsed() << "Preempting process: " << readyQ.top().getTau() - readyQ.top().getTimeElapsed() << std::endl;
                // std::cout << readyQ.top().getCPUBurstTime() << " " << readyQ.top().getRemainingBurstTime();
                return false;
            }
        }
        else
        {
            return false;
        }
    }
}
bool CPU::tSliceExpired()
{
    return CPU::cpuType == "RR" && runningProcess.getTimeElapsed() % CPU::tSlice == 0;
}

void CPU::preemptProcess(int curTime)
{
    runningProcess.setStatus("CTX_OUT");
    runningProcess.setBlock(curTime + ctxSwitch / 2);
}
void CPU::checkProcessArrival(int curTime, simout &statLogger)
{
    while (!cpuArrivals.empty() && curTime >= cpuArrivals.top().getArrivalTime())
    {

        Process newProcess = cpuArrivals.top();
        statLogger.logNewBurst(newProcess.getPID(), newProcess.getBoundType());
        statLogger.logBurstStart(newProcess.getPID(), curTime);
        cpuArrivals.pop();
        newProcess.setStatus("READY");
        readyQ.push(newProcess);
        statLogger.logBurstReady(newProcess.getPID(), curTime);
        reportProcessArrival(curTime, newProcess.getPID(), newProcess.getTau());
        if (canPreempt())
        {
            preemptProcess(curTime);
            statLogger.logPreemption(runningProcess.getPID());
            if (curTime < 9999)
                std::cout << " preempting " << runningProcess.getPID() << " " << this->printQ(readyQ) << std::endl;
        }
        else
        {
            if (curTime < 9999)
                std::cout << " added to ready queue " << this->printQ(readyQ) << std::endl;
        }
    }

    this->checkForReadyProcess(curTime, statLogger);
}

void CPU::checkProcessIODone(int curTime, simout &statLogger)
{

    while (!waitingQ.empty() && curTime >= waitingQ.top().blockedUntil())
    {
        Process newProcess = waitingQ.top();
        waitingQ.pop();
        newProcess.setStatus("READY");
        newProcess.incrementNumBurst();
        newProcess.setArrivalTime(curTime);
        newProcess.setArrivalStatus(1);
        newProcess.setRemainingBurstTime(newProcess.getCPUBurstTime());
        // std::cout << "Process " << newProcess.getPID() << "Comparison value is " << newProcess.getTau() - newProcess.getTimeElapsed() << std::endl;
        statLogger.logBurstReady(newProcess.getPID(), curTime);
        statLogger.logBurstStart(newProcess.getPID(), curTime);
        readyQ.push(newProcess);
        reportIODone(curTime, newProcess.getPID(), newProcess.getTau());
        if (canPreempt())
        {
            preemptProcess(curTime);
            if (curTime < 9999)
                std::cout << " preempting " << runningProcess.getPID() << " " << this->printQ(readyQ) << std::endl;
            statLogger.logPreemption(runningProcess.getPID());
        }
        else
        {
            if (curTime < 9999)
                std::cout << " added to ready queue " << this->printQ(readyQ) << std::endl;
            // this->checkForReadyProcess(curTime);
        }
    }
}
void CPU::checkForReadyProcess(int curTime, simout &statLogger)
{
    if (isRunning)
    {
        if (!canPreempt())
        {
            return;
        }
    }
    else
    {
        if (readyQ.size() > 0)
        {
            Process newProcess = readyQ.top();
            readyQ.pop();
            newProcess.setStatus("CTX_IN");
            // std::cout << "time "<< curTime << "Process "<< newProcess.getPID() << "CTX IN" << std::endl;
            newProcess.setBlock(curTime + (ctxSwitch / 2));

            // std::cout << "Time: " << curTime<<" Adding Process "<< newProcess.getPID() << " CTX_IN until " << newProcess.blockedUntil() << std::endl;
            runningProcess = newProcess;
            statLogger.logCTXIN(runningProcess.getPID(), curTime);
            isRunning = true;
        }
    }
}

void CPU::checkBurstCompleteSS(int curTime, simout &statLogger)
{
    if (!isRunning)
        return;
    if (runningProcess.getStatus() == "RUNNING")
    {
        if (runningProcess.blockedUntil() <= curTime)
        {
            if (runningProcess.getNumCPUBursts() - runningProcess.getCurrentBurst() - 1 == 0)
            {
                reportProcessTerminated(curTime, runningProcess.getPID());
                statLogger.logBurstComplete(runningProcess.getPID(), curTime + ctxSwitch / 2, runningProcess.getCPUBurstTime());
            }
            else
            {
                reportCPUBurstCompleted(curTime, runningProcess.getPID(), runningProcess.getNumCPUBursts() - runningProcess.getCurrentBurst() - 1, runningProcess.getTau());
                int oldTau = runningProcess.getTau();
                runningProcess.recalculateTau(runningProcess.getCPUBurstTime());
                reportRecalculatedTau(curTime, runningProcess.getPID(), oldTau, runningProcess.getTau());
                reportIOBlockStart(curTime, runningProcess.getPID(), curTime + runningProcess.getIOBurstTime() + ctxSwitch / 2);
            }
            runningProcess.setStatus("CTX_OUT");

            runningProcess.setBlock(curTime + ctxSwitch / 2);
        }
    }
    else if (runningProcess.getStatus() == "CTX_OUT")
    {
        if (runningProcess.blockedUntil() <= curTime)
        {
            statLogger.logCTXOUT(runningProcess.getPID());
            runningProcess.setBlock(curTime + runningProcess.getIOBurstTime());
            runningProcess.setStatus("IO");
            if (runningProcess.getNumCPUBursts() - runningProcess.getCurrentBurst() - 1 > 0 || runningProcess.getRemainingBurstTime() > 0)
            {
                // std::cout << runningProcess.getRemainingBurstTime() << std::endl;
                if (runningProcess.getRemainingBurstTime() <= 0)
                {
                    statLogger.logBurstComplete(runningProcess.getPID(), curTime, runningProcess.getCPUBurstTime());
                    waitingQ.push(runningProcess);
                }
                else
                {
                    // std::cout << "Remaining Burst Time " <<  runningProcess.getRemainingBurstTime() << std::endl;
                    runningProcess.setArrivalStatus(2);
                    runningProcess.setArrivalTime(curTime);
                    readyQ.push(runningProcess);
                    statLogger.logBurstReady(runningProcess.getPID(), curTime);
                }
            }
            isRunning = false;
            // this->checkForReadyProcess(curTime);
        }
    }
    else if (runningProcess.getStatus() == "CTX_IN")
    {
        if (runningProcess.blockedUntil() <= curTime)
        {
            runningProcess.setStatus("RUNNING");
            runningProcess.setBlock(curTime + runningProcess.getCPUBurstTime() - runningProcess.getTimeElapsed());
            if (runningProcess.getTimeElapsed() == 0)
            {
                reportCPUBurstStart(curTime, runningProcess.getPID(), runningProcess.getCPUBurstTime(), runningProcess.getTau());
            }
            else
            {
                reportCPUBurstContinue(curTime);
            }

            // runningProcess.decrementRemainingBurstTime();
        }
    }

    if (canPreempt())
    {
        if (curTime < 9999)
            std::cout << "	time " << curTime << "ms: Process " << readyQ.top().getPID() << "(tau "
                      << runningProcess.getTau() << "ms) will preempt " << runningProcess.getPID() << printQ(readyQ) << std::endl;
        statLogger.logPreemption(runningProcess.getPID());
        preemptProcess(curTime);
    }
}
void CPU::checkBurstCompleteFR(int curTime, simout &statLogger)
{
    if (!isRunning)
        return;
    if (runningProcess.getStatus() == "RUNNING")
    {
        if (runningProcess.blockedUntil() <= curTime)
        {
            if (runningProcess.getNumCPUBursts() - runningProcess.getCurrentBurst() - 1 == 0)
            {
                reportProcessTerminated(curTime, runningProcess.getPID());
                statLogger.logBurstComplete(runningProcess.getPID(), curTime + ctxSwitch / 2, runningProcess.getCPUBurstTime());
            }
            else
            {
                reportCPUBurstCompleted(curTime, runningProcess.getPID(), runningProcess.getNumCPUBursts() - runningProcess.getCurrentBurst() - 1, runningProcess.getTau());
                reportIOBlockStart(curTime, runningProcess.getPID(), curTime + runningProcess.getIOBurstTime() + ctxSwitch / 2);
            }
            runningProcess.setStatus("CTX_OUT");
            runningProcess.setBlock(curTime + ctxSwitch / 2);
        }
        else if (this->tSliceExpired())
        {
            if (curTime < 9999)
                std::cout << "time " << curTime << "ms: Time slice expired;";
            if (readyQ.size() > 0)
            {
                if (curTime < 9999)
                    std::cout << " preempting process " << runningProcess.getPID() << " with " << runningProcess.getRemainingBurstTime()
                              << "ms remaining " << this->printQ(readyQ) << std::endl;
                preemptProcess(curTime);
                statLogger.logPreemption(runningProcess.getPID());
            }
            else
            {
                if (curTime < 9999)
                    std::cout << " no preemption because ready queue is empty " << this->printQ(readyQ) << std::endl;
            }
        }
    }
    else if (runningProcess.getStatus() == "CTX_OUT")
    {
        if (runningProcess.blockedUntil() <= curTime)
        {
            statLogger.logCTXOUT(runningProcess.getPID());
            runningProcess.setBlock(curTime + runningProcess.getIOBurstTime());
            runningProcess.setStatus("IO");
            if (runningProcess.getNumCPUBursts() - runningProcess.getCurrentBurst() - 1 > 0 || runningProcess.getRemainingBurstTime() > 0)
            {
                // std::cout << runningProcess.getRemainingBurstTime() << std::endl;
                if (runningProcess.getRemainingBurstTime() <= 0)
                {
                    statLogger.logBurstComplete(runningProcess.getPID(), curTime, runningProcess.getCPUBurstTime());
                    waitingQ.push(runningProcess);
                }
                else
                {
                    // std::cout << "Remaining Burst Time " <<  runningProcess.getRemainingBurstTime() << std::endl;
                    runningProcess.setArrivalTime(curTime);
                    runningProcess.setArrivalStatus(2);
                    readyQ.push(runningProcess);
                    statLogger.logBurstReady(runningProcess.getPID(), curTime);
                }
            }
            isRunning = false;
            // this->checkForReadyProcess(curTime);
        }
    }
    else if (runningProcess.getStatus() == "CTX_IN")
    {
        if (runningProcess.blockedUntil() <= curTime)
        {
            runningProcess.setStatus("RUNNING");
            runningProcess.setBlock(curTime + runningProcess.getCPUBurstTime() - runningProcess.getTimeElapsed());
            if (runningProcess.getTimeElapsed() == 0)
            {
                reportCPUBurstStart(curTime, runningProcess.getPID(), runningProcess.getCPUBurstTime(), runningProcess.getTau());
            }
            else
                reportCPUBurstContinue(curTime);

            // runningProcess.decrementRemainingBurstTime();
        }
    }
}
void CPU::updateRunningProcess(simout &statLogger)
{
    if (!isRunning)
        return;
    if (runningProcess.getStatus() == "RUNNING")
    {
        runningProcess.decrementRemainingBurstTime();
        statLogger.logRunning();
    }

    return;
}
