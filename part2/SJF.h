#ifndef __SJF_H__
#define __SJF_H__

#include "Process.h"
#include "util.h"
#include <vector>

using namespace std;

void run_sjf(vector<Process> processes, int context_switch_time, float alpha, float lambda, StatisticsHelper &stats);

#endif // __SJF_H__