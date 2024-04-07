#include "Process.h"
#include <iostream>

using namespace std;

Process::Process(char id, int arrival_time, int cpu_burst_count, std::vector<int> cpu_bursts, std::vector<int> io_bursts, int tau)
    : id(id), arrival_time(arrival_time), cpu_burst_count(cpu_burst_count), cpu_bursts(cpu_bursts), io_bursts(io_bursts), tau(tau)
{
    cpu_current_burst_finish_time = cpu_bursts[0];
    io_current_burst_finish_time = io_bursts[0];
    cpu_current_burst_remaining_time = cpu_bursts[0];
    cpu_current_burst_remaining_time_dec = cpu_bursts[0];
    status = "WAITING";
}

Process::Process()
{
    status = "WAITING";
}

void Process::update_tau(float alpha)
{
    tau = ceil(alpha * cpu_current_burst_remaining_time + (1.0 - alpha) * tau);
}