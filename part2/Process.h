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
    int cpu_current_burst_remaining_time;
    int switch_time = 0;
    bool was_preempted = false;
    std::string status;
    Process(char id, int arrival_time, int cpu_burst_count, std::vector<int> cpu_bursts, std::vector<int> io_bursts);
    Process();
};

#endif // __PROCESS_H__
