#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <math.h>
// #include "random_gen.h"
#include <algorithm>
#include <queue>
#include <cassert>
#include <vector>

enum STATE
{
    RUNNING,
    READY,
    WAITING,
    TERMINATED,
    CONTEXT_SWITCH,
    NONE
};
enum EVENT
{
    START_CPU,
    FINISH_CPU,
    CONTEXT_SWITCH_OUT,
    START_IO,
    FINISH_IO,
    ARRIVE,
    CONTEXT_SWITCH_IN,
    ALL_DONE,
};
enum BOUND
{
    CPU,
    IO
};
enum PART
{
    PART1,
    PART2
};

struct Process
{
    // create constructor
    void setupProcess(STATE state_, BOUND bound_, char process_id_, double lambda, int up_bound)
    {
        state = state_;
        bound = bound_;
        process_id = process_id_;

        init_arrival_time = floor(next_exp(lambda, up_bound));
        burst_count = ceil(drand48() * 100);
        cpu_bursts = new int *[burst_count];
        for (int i = 0; i < burst_count; i++)
        {
            cpu_bursts[i] = new int[2];
            // CPU Burst time
            cpu_bursts[i][0] = ceil(next_exp(lambda, up_bound));
            // I/O burst time

            bool is_last_burst = i == burst_count - 1;

            // If last process then I/O burst time is -1 (none)
            cpu_bursts[i][1] = is_last_burst ? -1 : ceil(next_exp(lambda, up_bound)) * 10;

            // If bound is CPU, then mult CPU burst time by 4 and I/O burst time by 1/4
            if (bound == CPU)
            {
                cpu_bursts[i][0] *= 4;
                if (is_last_burst)
                {
                    cpu_bursts[i][1] = -1;
                }
                else
                {
                    cpu_bursts[i][1] /= 4;
                }
            }
        }
        curr_burst_to_run = 0;
        running_burst_remaining_time = cpu_bursts[0][0];
        event = ARRIVE;
        time_till_event = init_arrival_time;
        float XD = 1 / lambda;
        tau = ceil(XD);
        preempted = false;
    }
    // PART 1 variables
    char process_id;
    int init_arrival_time;
    int **cpu_bursts;
    int burst_count;
    STATE state;
    BOUND bound;
    // PART 2 variables
    EVENT event;
    int time_till_event;
    int curr_burst_to_run;            // keep track of what burst is to be run
    int running_burst_remaining_time; // how much time to left to complete curr_burst_to_run
    int tau;
    bool preempted;
};

bool process_comparator(const Process &p1, const Process &p2)
{
    return p1.init_arrival_time < p2.init_arrival_time;
}

class SRT_comparator
{
public:
    bool operator()(const Process *p1, const Process *p2)
    {
        if (p1->tau == p2->tau)
        {
            return p1->process_id > p2->process_id;
        }
        else
        {
            int p1_tau = p1->tau;
            if (p1->running_burst_remaining_time < p1->cpu_bursts[p1->curr_burst_to_run][0])
            {
                p1_tau -= (p1->cpu_bursts[p1->curr_burst_to_run][0] - p1->running_burst_remaining_time);
            }
            int p2_tau = p2->tau;
            if (p2->running_burst_remaining_time < p2->cpu_bursts[p2->curr_burst_to_run][0])
            {
                p2_tau -= (p2->cpu_bursts[p2->curr_burst_to_run][0] - p2->running_burst_remaining_time);
            }
            return p1_tau > p2_tau;
        }
    }
};

class SJF_comparator
{
public:
    bool operator()(const Process p1, const Process p2)
    {
        if (p1.tau == p2.tau)
        {
            return p1.process_id > p2.process_id;
        }
        else
        {
            int p1_tau = p1.tau;
            if (p1.running_burst_remaining_time < p1.cpu_bursts[p1.curr_burst_to_run][0])
            {
                p1_tau -= (p1.cpu_bursts[p1.curr_burst_to_run][0] - p1.running_burst_remaining_time);
            }
            int p2_tau = p2.tau;
            if (p2.running_burst_remaining_time < p2.cpu_bursts[p2.curr_burst_to_run][0])
            {
                p2_tau -= (p2.cpu_bursts[p2.curr_burst_to_run][0] - p2.running_burst_remaining_time);
            }
            return p1_tau > p2_tau;
        }
    }
};

bool process_event_time_comparator(const Process &p1, const Process &p2)
{
    /* TODO:
     * If different types of events occur at the same time, simulate these events in the following order:
     * (a) CPU burst completion; (b) process starts using the CPU; (c) I/O burst completions; and
     * (d) new process arrivals.
     */

    if (p1.time_till_event == p2.time_till_event)
    {
        if (p1.event == ARRIVE && p1.preempted == true)
        {
            return true;
        }
        if (p2.event == ARRIVE && p2.preempted == true)
        {
            return false;
        }

        if (p1.event == p2.event)
            return p1.process_id < p2.process_id;
        return p1.event < p2.event;
    }

    return p1.time_till_event < p2.time_till_event;
}

bool IO_comparator(const Process *p1, const Process *p2)
{
    return p1->process_id < p2->process_id;
}

// Error handling for reading in integers:
// Returns the integer if it is valid, otherwise returns -1
int read_int(char *inp)
{
    char *end;
    int x = strtol(inp, &end, 10);
    if (end == inp)
        return -1;
    return x;
}

double read_double(char *inp)
{
    char *end;
    double x = strtof(inp, &end);
    if (end == inp)
        return -1;
    return x;
}

void print_part1_header(int n, int n_cpu);
void print_processes(PART PART1, Process *processes, int num_processes);
void print_part2_header(int t_cs, float alpha, int t_slice);
void print_queue(std::queue<Process> q)
{
    if (q.empty())
    {
        std::cout << "[Q <empty>]\n";
    }
    else
    {
        std::cout << "[Q";
        while (!q.empty())
        {
            std::cout << " " << q.front().process_id;
            q.pop();
        }
        std::cout << "]\n";
    }
}

void print_SJF_queue(std::priority_queue<Process, std::vector<Process>, SJF_comparator> q)
{
    if (q.empty())
    {
        std::cout << "[Q <empty>]\n";
    }
    else
    {
        std::cout << "[Q";
        while (!q.empty())
        {
            std::cout << " " << q.top().process_id;
            q.pop();
        }
        std::cout << "]\n";
    }
}

void print_SRT_queue(std::priority_queue<Process *, std::vector<Process *>, SRT_comparator> q)
{
    if (q.empty())
    {
        std::cout << "[Q <empty>]\n";
    }
    else
    {
        std::cout << "[Q";
        while (!q.empty())
        {
            std::cout << " " << q.top()->process_id;
            q.pop();
        }
        std::cout << "]\n";
    }
}

int main(int argc, char **argv)
{

    if (argc != 9)
    {
        std::cerr << "ERROR: Invalid number of arguments\n"
                  << std::endl;
        exit(1);
    }

    int n = read_int(argv[1]);
    int n_cpu = read_int(argv[2]);
    int seed = read_int(argv[3]);
    double lambda = read_double(argv[4]);
    int up_bound = read_int(argv[5]);
    int t_cs = read_int(argv[6]); // context-switch time
    float alpha = read_double(argv[7]);
    int t_slice = read_int(argv[8]);

    if (n < 1 || n_cpu < 0 || seed < 0 || lambda <= 0 || up_bound <= 0 ||
        t_cs < 1 || t_cs % 2 == 1 || t_slice < 1 || alpha > 1 || alpha <= 0 ||
        lambda > 1)
    {
        std::cerr << "ERROR: Invalid input arguments\n"
                  << std::endl;
        exit(1);
    }

    // Generate seed for random number
    srand48(seed);

    // Generate processes based on n and the first n - n_cpu are IO bound
    Process *processes = new Process[n];
    for (char c = 65; c < 65 + n; c++)
    {
        if (c - 65 < n - n_cpu)
        {
            processes[c - 65].setupProcess(NONE, IO, c, lambda, up_bound);
        }
        else
        {
            processes[c - 65].setupProcess(NONE, CPU, c, lambda, up_bound);
        }
    }

    print_part1_header(n, n_cpu);
    print_processes(PART2, processes, n);
    std::cout << std::endl;
    print_part2_header(t_cs, alpha, t_slice);

    // FCFS
    std::cout << "time 0ms: Simulator started for FCFS [Q <empty>]" << std::endl;

    int terminated_process = 0;
    std::queue<Process> FCFS_ready_queue;

    std::vector<Process> FCFS_processes;
    for (int i = 0; i < n; i++)
    {
        FCFS_processes.push_back(processes[i]);
    }

    int simulation_time = 0;
    bool has_process_in_CPU = false;
    bool context_switch_occuring = false;
    while (terminated_process <= n)
    {
        std::sort(FCFS_processes.begin(), FCFS_processes.end(),
                  &process_event_time_comparator);

        int next_event_process_index;
        int time_change = 0;
        for (unsigned int i = 0; i < FCFS_processes.size(); i++)
        {
            Process *curr_process = &FCFS_processes[i];
            if (curr_process->state == TERMINATED && curr_process->event != CONTEXT_SWITCH_OUT)
                continue;

            if (curr_process->event == ARRIVE)
            {
                // Perform action -- Add this arriving CPU to readyQ
                time_change = curr_process->time_till_event;
                FCFS_ready_queue.push(*curr_process);
                if (simulation_time + time_change < 10000)
                {
                    std::cout << "time " << simulation_time + time_change << "ms: Process "
                              << curr_process->process_id << " arrived; added to ready queue ";
                    print_queue(FCFS_ready_queue);
                }

                // Update process state to next event
                curr_process->state = READY;
                curr_process->event = CONTEXT_SWITCH_IN;
                curr_process->time_till_event = 0;
                next_event_process_index = i;
                break;
            }
            else if (curr_process->event == CONTEXT_SWITCH_IN)
            {
                if (!has_process_in_CPU && !context_switch_occuring &&
                    curr_process->process_id == FCFS_ready_queue.front().process_id)
                {
                    context_switch_occuring = true;
                    FCFS_ready_queue.pop();
                    curr_process->event = START_CPU;
                    curr_process->time_till_event = t_cs / 2;
                    next_event_process_index = i;
                    break;
                }
                else
                {
                    continue;
                }
            }
            else if (curr_process->event == START_CPU)
            {
                if (!has_process_in_CPU)
                {
                    // Perform action -- context switch into CPU
                    has_process_in_CPU = true;
                    context_switch_occuring = false;
                    time_change = curr_process->time_till_event;
                    if (simulation_time + time_change < 10000)
                    {
                        std::cout << "time " << simulation_time + time_change << "ms: Process "
                                  << curr_process->process_id << " started using the CPU for "
                                  << curr_process->running_burst_remaining_time << "ms burst ";
                        print_queue(FCFS_ready_queue);
                    }
                    // Update process state to next event
                    curr_process->state = RUNNING;
                    curr_process->event = FINISH_CPU;
                    curr_process->time_till_event = curr_process->running_burst_remaining_time;
                    next_event_process_index = i;
                    break;
                }
                else // skip this event, either CPU is full or we are not the front of readyQ
                {
                    continue;
                }
            }
            else if (curr_process->event == FINISH_CPU)
            {
                // Perform action -- remove this process from CPU
                has_process_in_CPU = false;
                context_switch_occuring = true;
                time_change = curr_process->time_till_event;

                // Mark as terminated if all done with burst
                if (curr_process->curr_burst_to_run == curr_process->burst_count - 1)
                {
                    std::cout << "time " << simulation_time + time_change << "ms: Process "
                              << curr_process->process_id << " terminated ";
                    print_queue(FCFS_ready_queue);
                    curr_process->state = TERMINATED;
                    terminated_process++;
                }
                // If not terminated go to context switch out to go to I/O
                else
                {
                    // Perform action -- switch to run I/O
                    int num_burst_left = curr_process->burst_count - curr_process->curr_burst_to_run - 1;
                    if (simulation_time + time_change < 10000)
                    {
                        std::cout << "time " << simulation_time + time_change << "ms: Process "
                                  << curr_process->process_id << " completed a CPU burst; "
                                  << num_burst_left;
                        if (num_burst_left > 1)
                            std::cout << " bursts to go ";
                        else
                            std::cout << " burst to go ";
                        print_queue(FCFS_ready_queue);

                        std::cout << "time " << simulation_time + time_change << "ms: Process "
                                  << curr_process->process_id << " switching out of CPU; blocking on I/O until time "
                                  << simulation_time + time_change + (t_cs / 2) + curr_process->cpu_bursts[curr_process->curr_burst_to_run][1] << "ms ";
                        print_queue(FCFS_ready_queue);
                    }
                    curr_process->state = CONTEXT_SWITCH;
                }

                // Update process state to next event
                curr_process->event = CONTEXT_SWITCH_OUT;
                curr_process->time_till_event = (t_cs / 2);
                next_event_process_index = i;
                break;
            }
            else if (curr_process->event == CONTEXT_SWITCH_OUT)
            {
                assert(context_switch_occuring == true);
                //  All process terminated, this is the last context switch before simulation ends
                if (terminated_process == n)
                {
                    terminated_process++; // to break simulation loop
                }
                context_switch_occuring = false;
                time_change = curr_process->time_till_event;
                if (curr_process->state == TERMINATED)
                    curr_process->event = ALL_DONE;
                else
                {
                    curr_process->state = WAITING; // on I/O
                    curr_process->event = FINISH_IO;
                    curr_process->running_burst_remaining_time = curr_process->cpu_bursts[curr_process->curr_burst_to_run][1];
                    curr_process->time_till_event = curr_process->running_burst_remaining_time; // I/O time for curr burst
                }
                next_event_process_index = i;
                break;
            }
            else if (curr_process->event == FINISH_IO)
            {
                // Perform action -- add this process back to readyQ, increment to next burst to run
                curr_process->curr_burst_to_run += 1;
                curr_process->running_burst_remaining_time = curr_process->cpu_bursts[curr_process->curr_burst_to_run][0];
                time_change = curr_process->time_till_event;
                FCFS_ready_queue.push(*curr_process);
                if (simulation_time + time_change < 10000)
                {
                    std::cout << "time " << simulation_time + time_change << "ms: Process "
                              << curr_process->process_id << " completed I/O; added to ready queue ";
                    print_queue(FCFS_ready_queue);
                }

                // Update process state to next event
                curr_process->state = READY;
                curr_process->event = CONTEXT_SWITCH_IN;
                curr_process->time_till_event = 0;
                next_event_process_index = i;
                break;
            }
        }
        // Update other process thats not the min time
        for (int i = 0; i < (int)FCFS_processes.size(); i++)
        {
            if (i == next_event_process_index || FCFS_processes[i].state == TERMINATED)
                continue;
            // Other process is RUNNING_CPU/ RUNNING_IO/ WAITING TO ARRIVE
            if (FCFS_processes[i].event == FINISH_CPU ||
                FCFS_processes[i].event == FINISH_IO ||
                FCFS_processes[i].event == ARRIVE ||
                FCFS_processes[i].event == CONTEXT_SWITCH_OUT ||
                FCFS_processes[i].event == START_CPU)
            {
                FCFS_processes[i].time_till_event -= time_change;
            }
        }
        simulation_time += time_change;
    }
    std::cout << "time " << simulation_time << "ms: Simulator ended for FCFS [Q <empty>]\n\n";

    // SJF
    {
        std::cout << "time 0ms: Simulator started for SJF [Q <empty>]" << std::endl;

        terminated_process = 0;
        std::priority_queue<Process, std::vector<Process>, SJF_comparator> SJF_ready_queue;

        std::vector<Process> SJF_processes;
        for (int i = 0; i < n; i++)
        {
            SJF_processes.push_back(processes[i]);
        }

        simulation_time = 0;
        has_process_in_CPU = false;
        context_switch_occuring = false;
        while (terminated_process <= n)
        {
            std::sort(SJF_processes.begin(), SJF_processes.end(),
                      &process_event_time_comparator);

            int next_event_process_index;
            int time_change = 0;
            for (unsigned int i = 0; i < SJF_processes.size(); i++)
            {
                Process *curr_process = &SJF_processes[i];
                if (curr_process->state == TERMINATED && curr_process->event != CONTEXT_SWITCH_OUT)
                    continue;

                if (curr_process->event == ARRIVE)
                {
                    // Perform action -- Add this arriving CPU to readyQ
                    time_change = curr_process->time_till_event;
                    SJF_ready_queue.push(*curr_process);
                    if (simulation_time + time_change < 10000)
                    {
                        std::cout << "time " << simulation_time + time_change << "ms: Process "
                                  << curr_process->process_id << " (tau " << curr_process->tau << "ms) arrived; added to ready queue ";
                        print_SJF_queue(SJF_ready_queue);
                    }

                    // Update process state to next event
                    curr_process->state = READY;
                    curr_process->event = CONTEXT_SWITCH_IN;
                    curr_process->time_till_event = 0;
                    next_event_process_index = i;
                    break;
                }
                else if (curr_process->event == CONTEXT_SWITCH_IN)
                {
                    if (!has_process_in_CPU && !context_switch_occuring &&
                        curr_process->process_id == SJF_ready_queue.top().process_id)
                    {
                        context_switch_occuring = true;
                        SJF_ready_queue.pop();
                        curr_process->event = START_CPU;
                        curr_process->time_till_event = t_cs / 2;
                        next_event_process_index = i;
                        break;
                    }
                    else // skip this event, either CPU is full or we are not the front of readyQ
                    {
                        continue;
                    }
                }
                else if (curr_process->event == START_CPU)
                {

                    // Perform action -- context switch into CPU
                    context_switch_occuring = false;
                    has_process_in_CPU = true;
                    time_change = curr_process->time_till_event;
                    if (simulation_time + time_change < 10000)
                    {
                        std::cout << "time " << simulation_time + time_change << "ms: Process "
                                  << curr_process->process_id << " (tau " << curr_process->tau
                                  << "ms) started using the CPU for "
                                  << curr_process->running_burst_remaining_time << "ms burst ";
                        print_SJF_queue(SJF_ready_queue);
                    }

                    // Update process state to next event
                    curr_process->state = RUNNING;
                    curr_process->event = FINISH_CPU;
                    curr_process->time_till_event = curr_process->running_burst_remaining_time;
                    next_event_process_index = i;
                    break;
                }
                else if (curr_process->event == FINISH_CPU)
                {
                    // Perform action -- remove this process from CPU
                    has_process_in_CPU = false;
                    context_switch_occuring = true;
                    time_change = curr_process->time_till_event;

                    // Mark as terminated if all done with burst
                    if (curr_process->curr_burst_to_run == curr_process->burst_count - 1)
                    {
                        std::cout << "time " << simulation_time + time_change << "ms: Process "
                                  << curr_process->process_id << " terminated ";
                        print_SJF_queue(SJF_ready_queue);
                        curr_process->state = TERMINATED;
                        terminated_process++;
                    }
                    // If not terminated go to context switch out to go to I/O
                    else
                    {
                        // Perform action -- switch to run I/O
                        int num_burst_left = curr_process->burst_count - curr_process->curr_burst_to_run - 1;
                        if (simulation_time + time_change < 10000)
                        {
                            std::cout << "time " << simulation_time + time_change << "ms: Process "
                                      << curr_process->process_id << " (tau " << curr_process->tau << "ms) completed a CPU burst; "
                                      << num_burst_left;
                            if (num_burst_left > 1)
                                std::cout << " bursts to go ";
                            else
                                std::cout << " burst to go ";
                            print_SJF_queue(SJF_ready_queue);
                            std::cout << "time " << simulation_time + time_change << "ms: Recalculating tau for process "
                                      << curr_process->process_id << ": old tau " << curr_process->tau << "ms ==> ";
                        }
                        float tmp = alpha * curr_process->cpu_bursts[curr_process->curr_burst_to_run][0] + (1 - alpha) * curr_process->tau;
                        curr_process->tau = ceil(tmp);
                        if (simulation_time + time_change < 10000)
                        {
                            std::cout << "new tau " << curr_process->tau << "ms ";
                            print_SJF_queue(SJF_ready_queue);
                            std::cout << "time " << simulation_time + time_change << "ms: Process "
                                      << curr_process->process_id << " switching out of CPU; blocking on I/O until time "
                                      << simulation_time + time_change + (t_cs / 2) + curr_process->cpu_bursts[curr_process->curr_burst_to_run][1] << "ms ";
                            print_SJF_queue(SJF_ready_queue);
                        }
                        curr_process->state = CONTEXT_SWITCH;
                    }

                    // Update process state to next event
                    curr_process->event = CONTEXT_SWITCH_OUT;
                    curr_process->time_till_event = (t_cs / 2);
                    next_event_process_index = i;
                    break;
                }
                else if (curr_process->event == CONTEXT_SWITCH_OUT)
                {
                    assert(context_switch_occuring == true);
                    //  All process terminated, this is the last context switch before simulation ends
                    if (terminated_process == n)
                    {
                        terminated_process++; // to break simulation loop
                    }
                    context_switch_occuring = false;
                    time_change = curr_process->time_till_event;
                    if (curr_process->state == TERMINATED)
                        curr_process->event = ALL_DONE;
                    else
                    {
                        curr_process->state = WAITING; // on I/O
                        curr_process->event = FINISH_IO;
                        curr_process->running_burst_remaining_time = curr_process->cpu_bursts[curr_process->curr_burst_to_run][1];
                        curr_process->time_till_event = curr_process->running_burst_remaining_time; // I/O time for curr burst
                    }
                    next_event_process_index = i;
                    break;
                }
                else if (curr_process->event == FINISH_IO)
                {
                    // Perform action -- add this process back to readyQ, increment to next burst to run
                    curr_process->curr_burst_to_run += 1;
                    curr_process->running_burst_remaining_time = curr_process->cpu_bursts[curr_process->curr_burst_to_run][0];
                    time_change = curr_process->time_till_event;
                    SJF_ready_queue.push(*curr_process);
                    if (simulation_time + time_change < 10000)
                    {
                        std::cout << "time " << simulation_time + time_change << "ms: Process "
                                  << curr_process->process_id << " (tau " << curr_process->tau << "ms) completed I/O; added to ready queue ";
                        print_SJF_queue(SJF_ready_queue);
                    }

                    // Update process state to next event
                    curr_process->state = READY;
                    curr_process->event = CONTEXT_SWITCH_IN;
                    curr_process->time_till_event = 0;
                    next_event_process_index = i;
                    break;
                }
            }
            // Update other process thats not the min time
            for (int i = 0; i < (int)SJF_processes.size(); i++)
            {
                if (i == next_event_process_index || SJF_processes[i].state == TERMINATED)
                    continue;
                // Other process is RUNNING_CPU/ RUNNING_IO/ WAITING TO ARRIVE
                if (SJF_processes[i].event == FINISH_CPU ||
                    SJF_processes[i].event == FINISH_IO ||
                    SJF_processes[i].event == ARRIVE ||
                    SJF_processes[i].event == CONTEXT_SWITCH_OUT ||
                    SJF_processes[i].event == START_CPU)
                {
                    SJF_processes[i].time_till_event -= time_change;
                }
            }
            simulation_time += time_change;
        }
        std::cout << "time " << simulation_time << "ms: Simulator ended for SJF [Q <empty>]\n\n";
    }
    // SRT
    std::cout << "time 0ms: Simulator started for SRT [Q <empty>]" << std::endl;
    std::vector<Process> SRT_processes;
    for (int i = 0; i < n; i++)
    {
        SRT_processes.push_back(processes[i]);
    }
    std::sort(SRT_processes.begin(), SRT_processes.end(), &process_comparator);

    std::priority_queue<Process *, std::vector<Process *>, SRT_comparator> SRT_ready_queue;
    terminated_process = 0;
    simulation_time = 0;
    int next_process = 0;
    Process *curr_process = nullptr;
    std::vector<Process *> IO_blocked;
    int start_process = -1;
    int init_cpu = -1;

    while (terminated_process < n)
    {

        if (start_process == simulation_time)
        {
            if (simulation_time < 10000)
            {
                std::cout << "time " << simulation_time << "ms: Process "
                          << curr_process->process_id << " (tau " << curr_process->tau
                          << "ms) started using the CPU for ";
                if (curr_process->running_burst_remaining_time < curr_process->cpu_bursts[curr_process->curr_burst_to_run][0])
                {
                    std::cout << "remaining " << curr_process->running_burst_remaining_time << "ms of "
                              << curr_process->cpu_bursts[curr_process->curr_burst_to_run][0] << "ms burst ";
                }
                else
                {
                    std::cout << curr_process->cpu_bursts[curr_process->curr_burst_to_run][0] << "ms burst ";
                }
                print_SRT_queue(SRT_ready_queue);
            }
            curr_process->state = RUNNING;
            curr_process->running_burst_remaining_time++;
            start_process = -1;
        }

        if (init_cpu == simulation_time)
        {
            curr_process->running_burst_remaining_time = simulation_time + curr_process->cpu_bursts[curr_process->curr_burst_to_run][1];
            IO_blocked.push_back(curr_process);
            std::sort(IO_blocked.begin(), IO_blocked.end(), IO_comparator);
            curr_process = nullptr;
        }

        // loop through io_blocked using vector wihtout iterator
        for (auto it = IO_blocked.begin(); it != IO_blocked.end();)
        {
            if ((*it)->running_burst_remaining_time == simulation_time)
            {
                (*it)->state = READY;
                if (simulation_time < 10000)
                {
                    std::cout << "time " << simulation_time << "ms: Process "
                              << (*it)->process_id << " (tau " << (*it)->tau << "ms) completed I/O;";
                }
                (*it)->curr_burst_to_run++;

                int curr_process_time_left = (*it)->tau - 1;
                if (curr_process != nullptr && curr_process->state == RUNNING)
                {
                    curr_process_time_left =
                        curr_process->tau - (curr_process->cpu_bursts[curr_process->curr_burst_to_run][0] - curr_process->running_burst_remaining_time) - (t_cs / 2);
                }
                (*it)->running_burst_remaining_time = (*it)->cpu_bursts[(*it)->curr_burst_to_run][0];

                if ((*it)->tau < curr_process_time_left)
                {
                    // preempt
                    SRT_ready_queue.push((*it));
                    if (simulation_time < 10000)
                    {
                        std::cout << " preempting " << curr_process->process_id << " ";
                        print_SRT_queue(SRT_ready_queue);
                    }
                    SRT_ready_queue.pop();
                    (*it)->state = CONTEXT_SWITCH;
                    curr_process->state = READY;

                    curr_process->running_burst_remaining_time--;
                    SRT_ready_queue.push(curr_process);
                    curr_process = (*it);

                    start_process = simulation_time + t_cs;
                }
                else
                {
                    // no preempt
                    SRT_ready_queue.push((*it));
                    if (simulation_time < 10000)
                    {
                        std::cout << " added to ready queue ";
                        print_SRT_queue(SRT_ready_queue);
                    }
                }

                it = IO_blocked.erase(it);
            }
            else
            {
                ++it;
            }
        }

        if (curr_process != nullptr)
        {
            // running a CPU burst
            if (curr_process->state == RUNNING)
            {
                curr_process->running_burst_remaining_time--;
                if (curr_process->running_burst_remaining_time == 0)
                {
                    curr_process->burst_count--;
                    if (curr_process->burst_count == 0)
                    {
                        terminated_process++;
                        std::cout << "time " << simulation_time << "ms: Process "
                                  << curr_process->process_id << " terminated ";
                        print_SRT_queue(SRT_ready_queue);
                        simulation_time += t_cs / 2;
                        curr_process = nullptr;
                    }
                    else
                    {
                        if (simulation_time < 10000)
                        {
                            std::cout << "time " << simulation_time << "ms: Process "
                                      << curr_process->process_id << " (tau " << curr_process->tau << "ms) completed a CPU burst; "
                                      << curr_process->burst_count;
                            if (curr_process->burst_count == 1)
                            {
                                std::cout << " burst to go ";
                            }
                            else
                            {
                                std::cout << " bursts to go ";
                            }
                            print_SRT_queue(SRT_ready_queue);
                            std::cout << "time " << simulation_time << "ms: Recalculating tau for process "
                                      << curr_process->process_id << ": old tau " << curr_process->tau << "ms ==> ";
                        }
                        float XD = alpha * curr_process->cpu_bursts[curr_process->curr_burst_to_run][0] + (1 - alpha) * curr_process->tau;
                        curr_process->tau = ceil(XD);
                        if (simulation_time < 10000)
                        {
                            std::cout << "new tau " << curr_process->tau << "ms ";
                            print_SRT_queue(SRT_ready_queue);
                            std::cout << "time " << simulation_time << "ms: Process "
                                      << curr_process->process_id << " switching out of CPU; blocking on I/O until time "
                                      << (t_cs / 2) + simulation_time + curr_process->cpu_bursts[curr_process->curr_burst_to_run][1] << "ms ";
                        }
                        init_cpu = (t_cs / 2) + simulation_time;
                        if (simulation_time < 10000)
                        {
                            print_SRT_queue(SRT_ready_queue);
                        }
                        curr_process->state = WAITING;
                    }
                }
            }
        }

        // check preempt
        if (curr_process != nullptr)
        {
            if (curr_process->state == RUNNING)
            {
                int curr_process_time_left =
                    curr_process->tau - (curr_process->cpu_bursts[curr_process->curr_burst_to_run][0] - curr_process->running_burst_remaining_time);
                if (curr_process_time_left > SRT_ready_queue.top()->tau)
                {
                    // preempt!
                    Process *new_one = SRT_ready_queue.top();
                    if (simulation_time < 10000)
                    {
                        std::cout << "time " << simulation_time << "ms: Process "
                                  << new_one->process_id << " (tau " << new_one->tau
                                  << "ms) will preempt " << curr_process->process_id << " ";
                        print_SRT_queue(SRT_ready_queue);
                    }
                    SRT_ready_queue.pop();
                    curr_process->state = READY;
                    SRT_ready_queue.push(curr_process);
                    curr_process = new_one;
                    curr_process->state = CONTEXT_SWITCH;
                    start_process = simulation_time + t_cs;
                }
            }
        }

        if (next_process != -1)
        {
            if (SRT_processes[next_process].init_arrival_time <= simulation_time)
            {
                SRT_processes[next_process].state = READY;
                SRT_ready_queue.push(&SRT_processes[next_process]);
                if (simulation_time < 10000)
                {
                    std::cout << "time " << SRT_processes[next_process].init_arrival_time << "ms: Process "
                              << SRT_processes[next_process].process_id << " (tau "
                              << SRT_processes[next_process].tau
                              << "ms) arrived; added to ready queue ";
                    print_SRT_queue(SRT_ready_queue);
                }
                next_process++;
                if (next_process == n)
                {
                    next_process = -1;
                }
            }
        }

        if (curr_process == nullptr && !SRT_ready_queue.empty())
        {
            curr_process = SRT_ready_queue.top();
            SRT_ready_queue.pop();
            curr_process->state = CONTEXT_SWITCH;
            start_process = simulation_time + t_cs / 2;
        }

        simulation_time++;
    }

    std::cout << "time " << simulation_time - 1 << "ms: Simulator ended for SRT [Q <empty>]\n\n";

    // RR
    std::cout << "time 0ms: Simulator started for RR [Q <empty>]" << std::endl;

    terminated_process = 0;
    std::queue<Process> RR_ready_queue;

    std::vector<Process> RR_processes;
    for (int i = 0; i < n; i++)
    {
        RR_processes.push_back(processes[i]);
    }

    simulation_time = 0;
    has_process_in_CPU = false;
    context_switch_occuring = false;
    while (terminated_process <= n)
    {
        std::sort(RR_processes.begin(), RR_processes.end(),
                  &process_event_time_comparator);

        int next_event_process_index;
        int time_change = 0;
        for (unsigned int i = 0; i < RR_processes.size(); i++)
        {
            Process *curr_process = &RR_processes[i];
            if (curr_process->state == TERMINATED && curr_process->event != CONTEXT_SWITCH_OUT)
                continue;

            if (curr_process->event == ARRIVE)
            {
                // Perform action -- Add this arriving CPU to readyQ
                time_change = curr_process->time_till_event;
                RR_ready_queue.push(*curr_process);

                if (curr_process->preempted)
                {
                    curr_process->preempted = false;
                    context_switch_occuring = false;
                }
                else
                {
                    if (simulation_time + time_change < 10000)
                    {
                        std::cout << "time " << simulation_time + time_change << "ms: Process "
                                  << curr_process->process_id << " arrived; added to ready queue ";
                        print_queue(RR_ready_queue);
                    }
                }

                // Update process state to next event
                curr_process->state = READY;
                curr_process->event = CONTEXT_SWITCH_IN;
                curr_process->time_till_event = 0;
                next_event_process_index = i;
                break;
            }
            else if (curr_process->event == CONTEXT_SWITCH_IN)
            {
                if (!has_process_in_CPU && !context_switch_occuring &&
                    curr_process->process_id == RR_ready_queue.front().process_id)
                {
                    context_switch_occuring = true;
                    RR_ready_queue.pop();
                    curr_process->event = START_CPU;
                    curr_process->time_till_event = t_cs / 2;
                    next_event_process_index = i;
                    break;
                }
                else
                {
                    continue;
                }
            }
            else if (curr_process->event == START_CPU)
            {
                if (!has_process_in_CPU)
                {
                    // Perform action -- context switch into CPU
                    has_process_in_CPU = true;
                    context_switch_occuring = false;
                    time_change = curr_process->time_till_event;

                    if (simulation_time + time_change < 10000)
                    {
                        if (curr_process->running_burst_remaining_time !=
                            curr_process->cpu_bursts[curr_process->curr_burst_to_run][0])
                        {
                            std::cout << "time " << simulation_time + time_change << "ms: Process "
                                      << curr_process->process_id << " started using the CPU for remaining "
                                      << curr_process->running_burst_remaining_time << "ms of "
                                      << curr_process->cpu_bursts[curr_process->curr_burst_to_run][0]
                                      << "ms burst ";
                        }
                        else
                        {
                            std::cout << "time " << simulation_time + time_change << "ms: Process "
                                      << curr_process->process_id << " started using the CPU for "
                                      << curr_process->running_burst_remaining_time << "ms burst ";
                        }
                        print_queue(RR_ready_queue);
                    }

                    // Update process state to next event
                    curr_process->state = RUNNING;
                    curr_process->event = FINISH_CPU;
                    curr_process->time_till_event = std::min(t_slice, curr_process->running_burst_remaining_time);
                    next_event_process_index = i;
                    break;
                }
                else // skip this event, either CPU is full or we are not the front of readyQ
                {
                    continue;
                }
            }
            else if (curr_process->event == FINISH_CPU)
            {
                // We get preempted
                if (curr_process->running_burst_remaining_time > t_slice && !RR_ready_queue.empty())
                {
                    assert(has_process_in_CPU == true);
                    curr_process->running_burst_remaining_time = curr_process->running_burst_remaining_time - t_slice;
                    curr_process->preempted = true;

                    time_change = curr_process->time_till_event;
                    has_process_in_CPU = false;
                    context_switch_occuring = true;
                    if (simulation_time + time_change < 10000)
                    {
                        std::cout << "time " << simulation_time + time_change
                                  << "ms: Time slice expired; preempting process "
                                  << curr_process->process_id << " with "
                                  << curr_process->running_burst_remaining_time << "ms remaining ";
                        print_queue(RR_ready_queue);
                    }

                    curr_process->event = ARRIVE;
                    curr_process->time_till_event = t_cs / 2;
                    next_event_process_index = i;
                    break;
                }
                // We run again
                else if (curr_process->running_burst_remaining_time > t_slice && RR_ready_queue.empty())
                {
                    assert(has_process_in_CPU == true);
                    assert(context_switch_occuring == false);

                    time_change = t_slice;
                    curr_process->running_burst_remaining_time = curr_process->running_burst_remaining_time - t_slice;
                    curr_process->time_till_event = std::min(t_slice, curr_process->running_burst_remaining_time);

                    if (simulation_time + time_change < 10000)
                    {
                        std::cout << "time " << simulation_time + time_change
                                  << "ms: Time slice expired; no preemption because ready queue is empty [Q <empty>]\n";
                    }

                    next_event_process_index = i;
                    break;
                }
                else
                {
                    // The process completed its CPU burst within time_slice
                    // Perform action -- remove this process from CPU
                    has_process_in_CPU = false;
                    context_switch_occuring = true;
                    time_change = curr_process->time_till_event;

                    // Mark as terminated if all done with burst
                    if (curr_process->curr_burst_to_run == curr_process->burst_count - 1)
                    {
                        std::cout << "time " << simulation_time + time_change << "ms: Process "
                                  << curr_process->process_id << " terminated ";
                        print_queue(RR_ready_queue);
                        curr_process->state = TERMINATED;
                        terminated_process++;
                    }
                    // If not terminated go to context switch out to go to I/O
                    else
                    {
                        // Perform action -- switch to run I/O
                        int num_burst_left = curr_process->burst_count - curr_process->curr_burst_to_run - 1;
                        if (simulation_time + time_change < 10000)
                        {
                            std::cout << "time " << simulation_time + time_change << "ms: Process "
                                      << curr_process->process_id << " completed a CPU burst; "
                                      << num_burst_left;
                            if (num_burst_left > 1)
                                std::cout << " bursts to go ";
                            else
                                std::cout << " burst to go ";
                            print_queue(RR_ready_queue);

                            std::cout << "time " << simulation_time + time_change << "ms: Process "
                                      << curr_process->process_id << " switching out of CPU; blocking on I/O until time "
                                      << simulation_time + time_change + (t_cs / 2) + curr_process->cpu_bursts[curr_process->curr_burst_to_run][1] << "ms ";
                            print_queue(RR_ready_queue);
                        }
                        curr_process->state = CONTEXT_SWITCH;
                    }

                    // Update process state to next event
                    curr_process->event = CONTEXT_SWITCH_OUT;
                    curr_process->time_till_event = (t_cs / 2);
                    next_event_process_index = i;
                    break;
                }
            }
            else if (curr_process->event == CONTEXT_SWITCH_OUT)
            {
                assert(context_switch_occuring == true);
                //  All process terminated, this is the last context switch before simulation ends
                if (terminated_process == n)
                {
                    terminated_process++; // to break simulation loop
                }
                context_switch_occuring = false;
                time_change = curr_process->time_till_event;
                if (curr_process->state == TERMINATED)
                    curr_process->event = ALL_DONE;
                else
                {
                    curr_process->state = WAITING; // on I/O
                    curr_process->event = FINISH_IO;
                    curr_process->running_burst_remaining_time = curr_process->cpu_bursts[curr_process->curr_burst_to_run][1];
                    curr_process->time_till_event = curr_process->running_burst_remaining_time; // I/O time for curr burst
                }
                next_event_process_index = i;
                break;
            }
            else if (curr_process->event == FINISH_IO)
            {
                // Perform action -- add this process back to readyQ, increment to next burst to run
                curr_process->curr_burst_to_run += 1;
                curr_process->running_burst_remaining_time = curr_process->cpu_bursts[curr_process->curr_burst_to_run][0];
                time_change = curr_process->time_till_event;
                RR_ready_queue.push(*curr_process);
                if (simulation_time + time_change < 10000)
                {
                    std::cout << "time " << simulation_time + time_change << "ms: Process "
                              << curr_process->process_id << " completed I/O; added to ready queue ";
                    print_queue(RR_ready_queue);
                }

                // Update process state to next event
                curr_process->state = READY;
                curr_process->event = CONTEXT_SWITCH_IN;
                curr_process->time_till_event = 0;
                next_event_process_index = i;
                break;
            }
        }
        // Update other process thats not the min time
        for (int i = 0; i < (int)RR_processes.size(); i++)
        {
            if (i == next_event_process_index || RR_processes[i].state == TERMINATED)
                continue;
            // Other process is RUNNING_CPU/ RUNNING_IO/ WAITING TO ARRIVE
            if (RR_processes[i].event == FINISH_CPU ||
                RR_processes[i].event == FINISH_IO ||
                RR_processes[i].event == ARRIVE ||
                RR_processes[i].event == CONTEXT_SWITCH_OUT ||
                RR_processes[i].event == START_CPU)
            {
                RR_processes[i].time_till_event -= time_change;
            }
        }
        simulation_time += time_change;
    }
    std::cout << "time " << simulation_time << "ms: Simulator ended for RR [Q <empty>]\n";

    // delete all dynamic memory
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; processes[i].cpu_bursts[j][1] != -1; j++)
        {
            delete[] processes[i].cpu_bursts[j];
        }
        delete[] processes[i].cpu_bursts;
    }
}

// PRINTING FUNCTIONS

void print_part1_header(int n, int n_cpu)
{
    std::cout << "<<< PROJECT PART I -- process set (n="
              << n << ") with " << n_cpu << " CPU-bound";

    if (n_cpu == 1)
    {
        std::cout << " process";
    }
    else
    {
        std::cout << " processes";
    }

    std::cout << " >>>" << std::endl;
}

void print_processes(PART part, Process *processes, int num_processes)
{
    for (int i = 0; i < num_processes; i++)
    {
        if (processes[i].bound == IO)
        {
            std::cout << "I/O";
        }
        else
        {
            std::cout << "CPU";
        }
        std::cout << "-bound process " << processes[i].process_id << ":";
        std::cout << " arrival time " << processes[i].init_arrival_time << "ms; ";
        std::cout << processes[i].burst_count << " CPU bursts";
        if (part == PART2)
            std::cout << std::endl;

        if (part == PART1)
        {
            std::cout << ":" << std::endl;
            for (int j = 0; j < processes[i].burst_count; j++)
            {
                int *burst_times = processes[i].cpu_bursts[j];
                std::cout << "--> CPU burst " << burst_times[0] << "ms";
                if (burst_times[1] != -1)
                {
                    std::cout << " --> I/O burst " << burst_times[1] << "ms";
                }
                std::cout << std::endl;
            }
        }
    }
}

void print_part2_header(int t_cs, float alpha, int t_slice)
{
    // FORMAT:
    // <<< PROJECT PART II -- t_cs=4ms; alpha=0.75; t_slice=256ms >>>
    std::cout << "<<< PROJECT PART II -- t_cs="
              << t_cs << "ms; alpha=" << std::fixed << std::setprecision(2) << alpha << "; t_slice="
              << t_slice << "ms >>>" << std::endl;
}