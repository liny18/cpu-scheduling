#ifndef __UTIL_H__
#define __UTIL_H__

#include "Process.h"
#include <queue>
#include <vector>

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

string print_queue(const priority_queue<Process, vector<Process>, ReadyComparator> &queue);

#endif // __UTIL_H__