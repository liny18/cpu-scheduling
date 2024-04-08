#ifndef __UTIL_H__
#define __UTIL_H__

#include "Process.h"
#include <queue>
#include <vector>
#include <iostream>
#include <unordered_map>

using namespace std;

struct ArrivalComparator
{
  bool operator()(const Process &p1, const Process &p2) const
  {
    if (p1.arrival_time == p2.arrival_time)
    {
      return p1.id > p2.id;
    }
    return p1.arrival_time > p2.arrival_time;
  }
};

struct WaitingComparator
{
  bool operator()(const Process &p1, const Process &p2) const
  {
    if (p1.io_current_burst_finish_time == p2.io_current_burst_finish_time)
    {
      return p1.id > p2.id;
    }
    return p1.io_current_burst_finish_time > p2.io_current_burst_finish_time;
  }
};

struct ReadyComparator
{
  bool operator()(const Process &p1, const Process &p2) const
  {
    if (p1.arrival_time == p2.arrival_time)
    {
      if (p1.priority == p2.priority)
      {
        return p1.id > p2.id;
      }
      return p1.priority < p2.priority;
    }
    return p1.arrival_time > p2.arrival_time;
  }
};

struct ReadyComparatorSJF
{
  bool operator()(const Process &p1, const Process &p2) const
  {
    if (p1.tau == p2.tau)
    {
      return p1.id > p2.id;
    }
    return p1.tau > p2.tau;
  };
};

struct ReadyComparatorSRT
{
  bool operator()(const Process &p1, const Process &p2) const
  {
    // cout << "p1.id: " << p1.id << " p1.tau: " << p1.tau << " p1.curr: " << p1.cpu_bursts[p1.current_burst_index] << " p1.remain " << p1.cpu_current_burst_remaining_time_dec << endl;
    // cout << "p2.id: " << p2.id << " p2.tau: " << p2.tau << " p2.curr: " << p2.cpu_bursts[p2.current_burst_index] << " p2.remain " << p2.cpu_current_burst_remaining_time_dec << endl;
    if (p1.tau - (p1.cpu_bursts[p1.current_burst_index] - p1.cpu_current_burst_remaining_time_dec) == p2.tau - (p2.cpu_bursts[p2.current_burst_index] - p2.cpu_current_burst_remaining_time_dec))
    {
      return p1.id > p2.id;
    }
    return p1.tau - (p1.cpu_bursts[p1.current_burst_index] - p1.cpu_current_burst_remaining_time_dec) > p2.tau - (p2.cpu_bursts[p2.current_burst_index] - p2.cpu_current_burst_remaining_time_dec);
  };
};

struct StatisticsHelper {
  int total_time = 0; 
  int cpu_used_time = 0; 
  int total_cpu_burst_time = 0;
  int total_cpu_bursts = 0; 
  int cpu_bound_bursts = 0; 
  int io_bound_bursts = 0; 
  int cpu_bound_burst_time = 0; 
  int io_bound_burst_time = 0;
  
  unordered_map<char, int> entry_times;
  int total_wait_time = 0;
  int cpu_bound_wait_time = 0;
  int io_bound_wait_time = 0;
  void update_wait_time(char processId, int currentTime, bool isCpuBound) {
    if (entry_times.find(processId) != entry_times.end()) {
      int wait_time = currentTime - entry_times[processId];
      total_wait_time += wait_time;

      if (isCpuBound) {
        cpu_bound_wait_time += wait_time;
      } else {
        io_bound_wait_time += wait_time;
      }

      entry_times.erase(processId);
    }
  }

  int context_switches = 0; 
  int cpu_context_switches = 0; 
  int io_context_switches = 0; 

  int num_preemptions = 0;
  int cpu_num_preemptions = 0;
  int io_num_preemptions = 0;

};



string print_queue(const priority_queue<Process, vector<Process>, ReadyComparator> &queue);
string print_queue_sjf(const priority_queue<Process, vector<Process>, ReadyComparatorSJF> &queue);
string print_queue_srt(const priority_queue<Process, vector<Process>, ReadyComparatorSRT> &queue);

#endif // __UTIL_H__