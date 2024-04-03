#ifndef __MEASUREMENTS_H__
#define __MEASUREMENTS_H__

class Measurements
{
private:
    double total_burst_time;
    double total_wait_time;
    double total_turnaround_time;
    int total_preemptions;
    int total_context_switches;

public:
    Measurements() : total_burst_time(0), total_wait_time(0), total_turnaround_time(0), total_preemptions(0), total_context_switches(0) {}

    double getTotalBurstTime() const { return total_burst_time; }
    double getTotalWaitTime() const { return total_wait_time; }
    double getTotalTurnaroundTime() const { return total_turnaround_time; }
    int getTotalPreemptions() const { return total_preemptions; }
    int getTotalContextSwitches() const { return total_context_switches; }

    void addBurstTime(double burst_time) { total_burst_time += burst_time; }
    void addWaitTime(double wait_time) { total_wait_time += wait_time; }
    void addTurnaroundTime(double turnaround_time) { total_turnaround_time += turnaround_time; }
    void addPreemption() { total_preemptions++; }
    void addContextSwitch() { total_context_switches++; }
};

#endif // __MEASUREMENTS_H__