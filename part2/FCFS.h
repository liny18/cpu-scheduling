#ifndef __FCFS_H__
#define __FCFS_H__

#include "Process.h"
#include "util.h"
#include <vector>

using namespace std;

void run_fcfs(vector<Process> processes, int context_switch_time, StatisticsHelper &stats);

#endif // __FCFS_H__