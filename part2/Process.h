#ifndef PROCESS_H
#define PROCESS_H

#include <vector>

class Process
{
private:
    char id;
    int arrival_time = 0;
    int cpu_burst_count = 0;
    std::vector<int> cpu_bursts;
    std::vector<int> io_bursts;
    int preemption_count = 0;

public:
    Process(char id, int arrival_time, int cpu_burst_count, std::vector<int> cpu_bursts, std::vector<int> io_bursts);

    char getId() const;
    int getArrivalTime() const;
    int getCpuBurstCount() const;
    int getCpuBurst(int index) const;
    int getIoBurst(int index) const;
};

#endif // PROCESS_H
