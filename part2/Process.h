#ifndef __PROCESS_H__
#define __PROCESS_H__

#include <vector>
#include <string>
#include <iostream>

class Process
{
private:
    char id;
    bool cpu_bound;
    int arrival_time;
    int cpu_burst_count;
    std::vector<int> cpu_bursts;
    std::vector<int> io_bursts;
    std::string status;
    int tau;
    int current_burst_index;
    int remaining_burst_time;
    int buffer_time;

public:
    Process() : id(0), cpu_bound(0), arrival_time(0), cpu_burst_count(0), status(""), tau(0), current_burst_index(0), remaining_burst_time(0), buffer_time(0) {}
    Process(char id, int cpu_bound, int arrival_time, int cpu_burst_count, std::vector<int> cpu_bursts, std::vector<int> io_bursts) : id(id), cpu_bound(cpu_bound), arrival_time(arrival_time), cpu_burst_count(cpu_burst_count), cpu_bursts(cpu_bursts), io_bursts(io_bursts), status(""), tau(0), current_burst_index(0), remaining_burst_time(0), buffer_time(0)
    {
        std::cout << "IO BURTS SIZE ORZ : " << io_bursts.size() << std::endl;
    }

    char getId() const { return id; }
    int getArrivalTime() const { return arrival_time; }
    int getCpuBurstCount() const { return cpu_burst_count; }
    int getCpuBurst(int index) const { return cpu_bursts[index]; }
    int getCpuBurst() const { return cpu_bursts[current_burst_index]; }
    int getIoBurst(int index) const { return io_bursts[index]; }
    int getIoBurst() const
    {
        std::cout << current_burst_index << std::endl;
        std::cout << io_bursts.size() << std::endl;
        return io_bursts[current_burst_index];
    }
    std::string getStatus() const { return status; }
    int getTau() const { return tau; }
    int getCurrentBurstIndex() const { return current_burst_index; }
    int getRemainingBurstTime() const { return remaining_burst_time; }
    int getBufferTime() const { return buffer_time; }
    bool is_cpu_bound() const { return cpu_bound; }

    void setStatus(const std::string &newStatus) { status = newStatus; }
    void setTau(int newTau);
    void setCurrentBurstIndex(int newIndex) { current_burst_index = newIndex; }
    void setRemainingBurstTime(int time) { remaining_burst_time = time; }
    void setBufferTime(int time) { buffer_time = time; }
};

#endif // __PROCESS_H__
