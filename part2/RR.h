#ifndef __RR_H__
#define __RR_H__

#include "Process.h"
#include "util.h"
#include <vector>

using namespace std;

void run_rr(vector<Process> processes, int context_switch_time, int time_slice, StatisticsHelper &stats);

#endif // __RR_H__