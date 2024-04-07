#ifndef __UTIL_H__
#define __UTIL_H__

#include "Process.h"
#include <queue>
#include <vector>
#include <iostream>

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

string print_queue(const priority_queue<Process, vector<Process>, ReadyComparator> &queue);
string print_queue_sjf(const priority_queue<Process, vector<Process>, ReadyComparatorSJF> &queue);
string print_queue_srt(const priority_queue<Process, vector<Process>, ReadyComparatorSRT> &queue);

#endif // __UTIL_H__