#include "FCFS.h"
#include <cmath>
#include <iostream>
#include <queue>

void run_fcfs(vector<Process> processes, int t_cs, StatisticsHelper &stats)
{
    priority_queue<Process, vector<Process>, ArrivalComparator> arrival_queue;
    for (auto p : processes)
        arrival_queue.push(p);

    // deque<Process> ready_queue;
    priority_queue<Process, vector<Process>, ReadyComparator> ready_queue;
    priority_queue<Process, vector<Process>, WaitingComparator> waiting_queue;

    int curr_time = 0;
    Process current_process = Process();

    cout << "time " << curr_time << "ms: Simulator started for FCFS [Q <empty>]" << endl;

    while (true)
    {
        // cout << "time " << curr_time << " " << arrival_queue.size() << " " << ready_queue.size() << " " << waiting_queue.size() << endl;
        // terminate when to complete
        //(a) CPU burst completion; (b) process starts using the CPU; (c) I/O burst completions; and (d) new process arrivals
        string output = "";
        if (arrival_queue.empty() && ready_queue.empty() && waiting_queue.empty() && current_process.status == "TERMINATED")
        {
            cout << "time " << curr_time + t_cs / 2 - 1 << "ms: Simulator ended for FCFS [Q <empty>]" << endl;
            break;
        }

        // if(current_process.status == "RUNNING") stats.cpu_used_time++; 

        // CPU BURST COMPLETION:
        if (current_process.status == "RUNNING" && curr_time >= current_process.cpu_current_burst_finish_time)
        {
            stats.total_cpu_burst_time += current_process.cpu_bursts[current_process.current_burst_index];
            stats.total_cpu_bursts++;
            if (current_process.cpu_bound) {
                stats.cpu_bound_burst_time += current_process.cpu_bursts[current_process.current_burst_index];
                stats.cpu_bound_bursts++;
            } else {
                stats.io_bound_burst_time += current_process.cpu_bursts[current_process.current_burst_index];
                stats.io_bound_bursts++;
            }

            if (current_process.current_burst_index == current_process.cpu_burst_count - 1)
            {
                // output += "time " + to_string(curr_time) + "ms: PROCESS " + current_process.id + " terminated\n";
                current_process.status = "TERMINATED";
                cout << "time " << curr_time << "ms: Process " << current_process.id << " terminated ";
                string o = print_queue(ready_queue);
                cout << o << endl;
                current_process.switch_time = curr_time + t_cs / 2;

            }
            else
            {
                current_process.status = "SWITCH_OUT";
                current_process.switch_time = curr_time + t_cs / 2;
                output += "time " + to_string(curr_time) + "ms: Process " + current_process.id + " completed a CPU burst; ";
                output += to_string((current_process.cpu_burst_count - current_process.current_burst_index - 1));
                string b = " ";
                if ((current_process.cpu_burst_count - current_process.current_burst_index - 1) == 1)
                {
                    b = " burst";
                }
                else
                {
                    b = " bursts";
                }
                output += b + " to go ";
                output += print_queue(ready_queue) + "\n";
                output += "time " + to_string(curr_time) + "ms: Process " + current_process.id + " switching out of CPU; blocking on I/O until time ";
                // might have issue, not sure why io_current_burst_finish_time won't work here
                output += to_string(current_process.switch_time + current_process.io_bursts[current_process.current_burst_index]) + "ms ";
                output += print_queue(ready_queue) + "\n";
            }
        }
        else if (current_process.status == "SWITCH_OUT")
        {
            if (curr_time >= current_process.switch_time)
            {
                current_process.status = "WAITING";
                current_process.arrival_time = curr_time;
                current_process.io_current_burst_finish_time = curr_time + current_process.io_bursts[current_process.current_burst_index];
                waiting_queue.push(current_process);
            }
        }

        // PROCESS STARTS USING CPU
        if (current_process.status == "SWITCH_IN")
        {
            if (curr_time >= current_process.switch_time)
            {
                stats.context_switches++;
                if(current_process.cpu_bound) stats.cpu_context_switches++;
                else stats.io_context_switches++;
                
                stats.update_wait_time(current_process.id, curr_time - t_cs / 2, current_process.cpu_bound);
                current_process.status = "RUNNING";
                current_process.cpu_current_burst_finish_time = curr_time + current_process.cpu_bursts[current_process.current_burst_index];
                output += "time " + to_string(curr_time) + "ms: Process " + current_process.id + " started using the CPU for ";
                output += to_string(current_process.cpu_bursts[current_process.current_burst_index]) + "ms burst ";
                output += print_queue(ready_queue) + "\n";
            }
        }

        // I/O BURST COMPLETION
        while (!waiting_queue.empty() && waiting_queue.top().io_current_burst_finish_time <= curr_time)
        {
            Process temp = waiting_queue.top();
            temp.status = "READY";
            temp.arrival_time = curr_time;
            temp.current_burst_index = temp.current_burst_index + 1;
            // temp.cpu_current_burst_finish_time = curr_time + temp.cpu_bursts[temp.current_burst_index];
            ready_queue.push(temp);
            stats.entry_times[temp.id] = curr_time; 
            waiting_queue.pop();
            output += "time " + to_string(curr_time) + "ms: Process " + temp.id + " completed I/O; added to ready queue ";
            output += print_queue(ready_queue) + "\n";
        }

        // NEW PROCESS ARRIVALS
        while (!arrival_queue.empty() && arrival_queue.top().arrival_time <= curr_time)
        {
            Process temp = arrival_queue.top();
            temp.status = "READY";
            // temp.cpu_current_burst_finish_time = curr_time + temp.cpu_bursts[temp.current_burst_index];
            temp.arrival_time = curr_time;
            ready_queue.push(temp);
            stats.entry_times[temp.id] = curr_time; 
            arrival_queue.pop();
            output += "time " + to_string(curr_time) + "ms: Process " + temp.id + " arrived; added to ready queue ";
            output += print_queue(ready_queue) + "\n";
        }

        // check if current process is not running
        if (!ready_queue.empty() && (current_process.status == "WAITING" || (current_process.status == "TERMINATED" && curr_time >= current_process.switch_time) || (current_process.status == "SWITCH_OUT" && curr_time >= current_process.switch_time)))
        {
            Process temp = ready_queue.top();
            temp.status = "SWITCH_IN";
            temp.switch_time = curr_time + t_cs / 2;
            ready_queue.pop();
            current_process = temp;
        }

        if (curr_time < 10000)
        {
            cout << output;
        }

        curr_time++;
    }

    stats.total_time = curr_time;

}
