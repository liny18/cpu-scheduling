#ifndef __SJF_H__
#define __SJF_H__

#include "Process.h"
#include <vector>

using namespace std;

void run_sjf(vector<Process> processes, int context_switch_time, float alpha, float lambda);

#endif // __SJF_H__