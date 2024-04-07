#include "SRT.h"
#include <cmath>
#include <iostream>
#include <queue>

using namespace std;

bool gotta_preempt(Process a, priority_queue<Process, vector<Process>, ReadyComparatorSRT> ready_queue)
{
    return a.tau - (a.cpu_bursts[a.current_burst_index] - a.cpu_current_burst_remaining_time_dec) > ready_queue.top().tau - (ready_queue.top().cpu_bursts[ready_queue.top().current_burst_index] - ready_queue.top().cpu_current_burst_remaining_time_dec);
}

void run_srt(vector<Process> processes, int t_cs, float alpha, float lambda, StatisticsHelper &stats)
{
    priority_queue<Process, vector<Process>, ArrivalComparator> arrival_queue;
    for (auto p : processes)
        arrival_queue.push(p);
    priority_queue<Process, vector<Process>, ReadyComparatorSRT> ready_queue;
    priority_queue<Process, vector<Process>, WaitingComparator> waiting_queue;

    int curr_time = 0;
    Process current_process = Process();
    current_process.tau = ceil(1 / lambda);

    cout << "time " << curr_time << "ms: Simulator started for SRT [Q <empty>]" << endl;

    while (true)
    {
        //     while (!temp_ready_queue.empty())
        //     {
        //         Process temp = temp_ready_queue.top();
        //         cout << temp.id << " " << temp.arrival_time << " " << temp.cpu_burst_count << " " << endl;
        //         temp_ready_queue.pop();
        //     }
        //     // cout << endl;
        // }
        // cout << "time " << curr_time << " " << arrival_queue.size() << " " << ready_queue.size() << " " << waiting_queue.size() << endl;
        // terminate when to complete
        //(a) CPU burst completion; (b) process starts using the CPU; (c) I/O burst completions; and (d) new process arrivals
        string output = "";
        if (arrival_queue.empty() && ready_queue.empty() && waiting_queue.empty() && current_process.status == "TERMINATED")
        {
            cout << "time " << curr_time + t_cs / 2 - 1 << "ms: Simulator ended for SRT [Q <empty>]" << endl;
            break;
        }

        if (current_process.status == "RUNNING")
        {
            stats.cpu_used_time++;
        }

        // CPU BURST COMPLETION:
        if (current_process.status == "RUNNING" && curr_time >= current_process.cpu_current_burst_finish_time)
        {
            if (current_process.current_burst_index == current_process.cpu_burst_count - 1)
            {
                // output += "time " + to_string(curr_time) + "ms: PROCESS " + current_process.id + " terminated\n";
                current_process.status = "TERMINATED";
                // stats.cpu_used_time++;
                cout << "time " << curr_time << "ms: Process " << current_process.id << " terminated ";
                string o = print_queue_srt(ready_queue);
                cout << o << endl;
                // NEED TO FIGURE OUT HOW TO ACTUAL TERMINATE
                current_process.switch_time = curr_time + t_cs / 2;
            }
            else
            {
                current_process.status = "SWITCH_OUT";
                current_process.switch_time = curr_time + t_cs / 2;
                output += "time " + to_string(curr_time) + "ms: Process " + current_process.id + " (tau " + to_string(current_process.tau) + "ms) " + "completed a CPU burst; ";
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
                output += print_queue_srt(ready_queue) + "\n";
                output += "time " + to_string(curr_time) + "ms: Recalculating tau for process ";
                output += current_process.id;
                output += ": old tau ";
                output += to_string(current_process.tau);
                output += "ms ==> new tau ";
                current_process.update_tau(alpha);
                output += to_string(current_process.tau);
                output += "ms ";
                std::string o = print_queue_srt(ready_queue);
                output += o + "\n";
                output += "time " + to_string(curr_time) + "ms: Process " + current_process.id + " switching out of CPU; blocking on I/O until time ";
                // might have issue, not sure why io_current_burst_finish_time won't work here
                output += to_string(current_process.switch_time + current_process.io_bursts[current_process.current_burst_index]) + "ms ";
                // output += "ms ";
                output += print_queue_srt(ready_queue) + "\n";
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
        else if (current_process.status == "PREEMPTED")
        {
            if (curr_time >= current_process.switch_time)
            {
                current_process.status = "READY";
                current_process.arrival_time = curr_time;
                current_process.was_preempted = true;
                ready_queue.push(current_process);
            }
        }

        // PROCESS STARTS USING CPU
        if (current_process.status == "SWITCH_IN")
        {
            if (curr_time >= current_process.switch_time)
            {
                current_process.status = "RUNNING";
                if (current_process.was_preempted)
                {
                    current_process.cpu_current_burst_finish_time = curr_time + current_process.cpu_current_burst_remaining_time_dec;
                    current_process.was_preempted = false;
                    if (current_process.cpu_current_burst_remaining_time_dec == current_process.cpu_bursts[current_process.current_burst_index])
                    {
                        output += "time " + to_string(curr_time) + "ms: Process " + current_process.id + " (tau " + to_string(current_process.tau) + "ms) " + "started using the CPU for ";
                        output += to_string(current_process.cpu_bursts[current_process.current_burst_index]) + "ms burst ";
                    }
                    else
                    {
                        output += "time " + to_string(curr_time) + "ms: Process " + current_process.id + " (tau " + to_string(current_process.tau) + "ms) " + "started using the CPU for remaining ";
                        output += to_string(current_process.cpu_current_burst_remaining_time_dec) + "ms of " + to_string(current_process.cpu_bursts[current_process.current_burst_index]) + "ms burst ";
                    }
                }
                else
                {
                    current_process.cpu_current_burst_finish_time = curr_time + current_process.cpu_bursts[current_process.current_burst_index];
                    current_process.cpu_current_burst_remaining_time = current_process.cpu_bursts[current_process.current_burst_index];
                    current_process.cpu_current_burst_remaining_time_dec = current_process.cpu_bursts[current_process.current_burst_index];
                    output += "time " + to_string(curr_time) + "ms: Process " + current_process.id + " (tau " + to_string(current_process.tau) + "ms) " + "started using the CPU for ";
                    output += to_string(current_process.cpu_bursts[current_process.current_burst_index]) + "ms burst ";
                }
                output += print_queue_srt(ready_queue) + "\n";
            }
            if (current_process.cpu_bursts.size() != 0 && !ready_queue.empty() && gotta_preempt(current_process, ready_queue) && current_process.status == "RUNNING")
            {
                output += "time " + to_string(curr_time) + "ms: Process " + ready_queue.top().id + " (tau " + to_string(ready_queue.top().tau) + "ms) will preempt " + current_process.id + " ";
                string o = print_queue_srt(ready_queue);
                output += o + "\n";
                current_process.status = "PREEMPTED";
                current_process.switch_time = curr_time + t_cs / 2;
            }
        }

        // I/O BURST COMPLETION
        while (!waiting_queue.empty() && waiting_queue.top().io_current_burst_finish_time <= curr_time)
        {
            Process temp = waiting_queue.top();
            temp.status = "READY";
            temp.arrival_time = curr_time;
            temp.current_burst_index = temp.current_burst_index + 1;
            temp.cpu_current_burst_remaining_time = temp.cpu_bursts[temp.current_burst_index];
            temp.cpu_current_burst_remaining_time_dec = temp.cpu_bursts[temp.current_burst_index];
            // temp.cpu_current_burst_finish_time = curr_time + temp.cpu_bursts[temp.current_burst_index];
            ready_queue.push(temp);
            output += "time " + to_string(curr_time) + "ms: Process " + temp.id + " (tau " + to_string(waiting_queue.top().tau) + "ms)";
            waiting_queue.pop();
            output += " completed I/O; ";

            if (current_process.cpu_bursts.size() != 0 && gotta_preempt(current_process, ready_queue) && current_process.status == "RUNNING")
            {
                current_process.status = "PREEMPTED";
                current_process.switch_time = curr_time + t_cs / 2;
                output += "preempting ";
                output += current_process.id;
                output += " ";
            }
            else
            {
                output += "added to ready queue ";
            }
            string o = print_queue_srt(ready_queue);
            output += o + "\n";
        }

        // NEW PROCESS ARRIVALS
        while (!arrival_queue.empty() && arrival_queue.top().arrival_time <= curr_time)
        {
            Process temp = arrival_queue.top();
            temp.status = "READY";
            // temp.cpu_current_burst_finish_time = curr_time + temp.cpu_bursts[temp.current_burst_index];
            temp.arrival_time = curr_time;
            ready_queue.push(temp);
            arrival_queue.pop();
            output += "time " + to_string(curr_time) + "ms: Process " + temp.id + " (tau " + to_string(temp.tau) + "ms)";
            output += " arrived; ";

            if (current_process.cpu_bursts.size() != 0 && gotta_preempt(current_process, ready_queue) && current_process.status == "RUNNING")
            {
                current_process.status = "PREEMPTED";
                current_process.switch_time = curr_time + t_cs / 2;
                output += "preempting ";
                output += current_process.id;
                output += " ";
            }
            else
            {
                output += "added to ready queue ";
            }
            string o = print_queue_srt(ready_queue);
            output += o + "\n";
        }

        // check if current process is not running
        if (!ready_queue.empty() && ((current_process.status == "WAITING" || (current_process.status == "TERMINATED" && curr_time >= current_process.switch_time) || (current_process.status == "SWITCH_OUT" && curr_time >= current_process.switch_time) || (current_process.status == "PREEMPTED" && curr_time >= current_process.switch_time)) || (current_process.status == "READY" && current_process.was_preempted)))
        {
            Process temp = ready_queue.top();
            temp.status = "SWITCH_IN";
            temp.switch_time = curr_time + t_cs / 2;
            ready_queue.pop();
            current_process = temp;
        }

        if (current_process.status == "RUNNING")
        {
            current_process.cpu_current_burst_remaining_time_dec--;
        }

        if (curr_time < 10000)
        {
            cout << output;
        }

        curr_time++;
    }

    stats.total_time = curr_time;
}
