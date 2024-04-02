#ifndef __PRINT__
#define __PRINT__

void print(int mode, char * algorithm, Process* processes, double wait_time, int ct_switch, int preem, double util, int total_processes, double tot_wait_time, int ct_time);
void print(int mode, char * algorithm, Process* processes, double wait_time, int ct_switch, int preem, double util, int total_processes, double tot_wait_time, int ct_time) {
    FILE * out;
    if(mode == 0) {
        out = fopen("simout.txt", "w");
    } else {
        out = fopen("simout.txt", "a");
    }
    double tot_burst_time = 0;
    double burst_time = 0;
    double sum_num_bursts = 0;
    for(int i = 0;i < total_processes; i++) {
        for(int j = 0; j < processes[i].total_bursts; j++) {
            burst_time += processes[i].cpu_burst_times[j];
            tot_burst_time += processes[i].cpu_burst_times[j];
        }
        sum_num_bursts += processes[i].total_bursts;
    }
    burst_time = burst_time / sum_num_bursts ;
    fprintf(out, "Algorithm %s\n", algorithm);
    fprintf(out, "-- average CPU burst time: %0.3f ms\n", ceil((burst_time) * 1000.0) / 1000.0);
    fprintf(out, "-- average wait time: %0.3f ms\n", ceil((wait_time) * 1000.0) / 1000.0);
    fprintf(out, "-- average turnaround time: %0.3f ms\n", ceil(((tot_burst_time + tot_wait_time + (ct_time * sum_num_bursts)) / sum_num_bursts) * 1000.0) / 1000.0);
    fprintf(out, "-- total number of context switches: %d\n", ct_switch);
    fprintf(out, "-- total number of preemptions: %d\n", preem);
    fprintf(out, "-- CPU utilization: %0.3f%%\n", ceil((util * 100) * 1000.0) / 1000.0);
    fclose(out);
}

int ableToPrint(int time) {
    if (time > 1000) {
        return false;
    } else {
        return true;
    }
}

#endif