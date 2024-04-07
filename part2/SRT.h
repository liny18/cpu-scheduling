#ifndef __SRT_H__
#define __SRT_H__

#include "Process.h"
#include <vector>

using namespace std;

void run_srt(vector<Process> processes, int context_switch_time, float alpha, float lambda);

#endif // __SJF_H__