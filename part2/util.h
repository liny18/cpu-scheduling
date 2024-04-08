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

  int context_switches = 0; 
  int cpu_context_switches = 0; 
  int io_context_switches = 0; 

  int num_preemptions = 0;
  int cpu_num_preemptions = 0;
  int io_num_preemptions = 0;

  int total_wait_time = 0;
  int cpu_bound_wait_time = 0;
  int io_bound_wait_time = 0;
  unordered_map<char, int> entry_times;

  void update_wait_time(char p_id, int curr_time, bool cpu_bound) {
    if (entry_times.find(p_id) != entry_times.end()) {
      int wait_time = curr_time - entry_times[p_id];
      total_wait_time += wait_time;
      if(cpu_bound) cpu_bound_wait_time += wait_time;
      else io_bound_wait_time += wait_time;
      entry_times.erase(p_id);
    }
  }
};



string print_queue(const priority_queue<Process, vector<Process>, ReadyComparator> &queue);
string print_queue_sjf(const priority_queue<Process, vector<Process>, ReadyComparatorSJF> &queue);
string print_queue_srt(const priority_queue<Process, vector<Process>, ReadyComparatorSRT> &queue);

#endif // __UTIL_H__