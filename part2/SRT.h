#ifndef __SRT_H__
#define __SRT_H__

#include "Process.h"
#include "util.h"
#include <vector>

using namespace std;

void run_srt(vector<Process> processes, int context_switch_time, float alpha, float lambda, StatisticsHelper &stats);

#endif // __SJF_H__