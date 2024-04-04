#ifndef PROCESS_H
#define PROCESS_H

#include <vector>

class Process
{
public:
    char id;
    int arrival_time = 0;
    int cpu_burst_count = 0;
    std::vector<int> cpu_bursts;
    std::vector<int> io_bursts;
    int preemption_count = 0;
    Process(char id, int arrival_time, int cpu_burst_count, std::vector<int> cpu_bursts, std::vector<int> io_bursts);
};

#endif // PROCESS_H
