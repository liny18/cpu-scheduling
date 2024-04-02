#include "Process.h"

Process::Process(char id, int arrival_time, int cpu_burst_count, std::vector<int> cpu_bursts, std::vector<int> io_bursts)
    : id(id), arrival_time(arrival_time), cpu_burst_count(cpu_burst_count), cpu_bursts(cpu_bursts), io_bursts(io_bursts)
{
}

char Process::getId() const
{
    return id;
}

int Process::getArrivalTime() const
{
    return arrival_time;
}

int Process::getCpuBurstCount() const
{
    return cpu_burst_count;
}

int Process::getCpuBurst(int index) const
{
    return cpu_bursts[index];
}

int Process::getIoBurst(int index) const
{
    return io_bursts[index];
}
