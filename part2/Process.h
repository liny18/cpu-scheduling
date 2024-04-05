#ifndef __PROCESS_H__
#define __PROCESS_H__

#include <vector>
#include <string>

class Process
{
public:
    char id;
    int arrival_time = 0;
    int cpu_burst_count = 0;
    std::vector<int> cpu_bursts;
    std::vector<int> io_bursts;
    int preemption_count = 0;
    int current_burst_index = 0;
    int cpu_current_burst_finish_time;
    int io_current_burst_finish_time;
    int switch_time = 0;
    std::string status;
    Process(char id, int arrival_time, int cpu_burst_count, std::vector<int> cpu_bursts, std::vector<int> io_bursts);
    Process();
};

Process::Process(char id, int arrival_time, int cpu_burst_count, std::vector<int> cpu_bursts, std::vector<int> io_bursts)
    : id(id), arrival_time(arrival_time), cpu_burst_count(cpu_burst_count), cpu_bursts(cpu_bursts), io_bursts(io_bursts)
{
    cpu_current_burst_finish_time = cpu_bursts[0];
    io_current_burst_finish_time = io_bursts[0];
    status = "WAITING";
}

Process::Process()
{
    status = "WAITING";
}

#endif // __PROCESS_H__
