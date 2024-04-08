#include "RR.h"
#include <cmath>
#include <iostream>
#include <queue>

bool gotta_preempt(Process a, int t_slice)
{
  return (a.cpu_bursts[a.current_burst_index] - a.cpu_current_burst_remaining_time) % t_slice == 0;
}

void run_rr(vector<Process> processes, int t_cs, int t_slice, StatisticsHelper &stats)
{
  priority_queue<Process, vector<Process>, ArrivalComparator>
      arrival_queue;
  for (auto p : processes)
    arrival_queue.push(p);
  priority_queue<Process, vector<Process>, ReadyComparator> ready_queue;
  priority_queue<Process, vector<Process>, WaitingComparator> waiting_queue;

  int curr_time = 0;
  Process current_process = Process();

  cout << "time " << curr_time << "ms: Simulator started for RR [Q <empty>]" << endl;

  while (true)
  {
    // ouput everything in ready queue
    //  if (curr_time >= 287 && curr_time <= 300) {
    //    priority_queue<Process, vector<Process>, ReadyComparator> temp_ready_queue = ready_queue;
    //    while (!temp_ready_queue.empty())
    //    {
    //      Process temp = temp_ready_queue.top();
    //      cout << temp.id << " " << temp.arrival_time << " " << temp.cpu_burst_count << " ";
    //      temp_ready_queue.pop();
    //    }
    //    cout << endl;
    //  }
    //  cout << "time " << curr_time << " " << arrival_queue.size() << " " << ready_queue.size() << " " << waiting_queue.size() << endl;
    //  terminate when to complete
    //(a) CPU burst completion; (b) process starts using the CPU; (c) I/O burst completions; and (d) new process arrivals
    string output = "";
    if (arrival_queue.empty() && ready_queue.empty() && waiting_queue.empty() && current_process.status == "TERMINATED")
    {
      cout << "time " << curr_time + t_cs / 2 - 1 << "ms: Simulator ended for RR [Q <empty>]" << endl;
      break;
    }

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
        // NEED TO FIGURE OUT HOW TO ACTUAL TERMINATE
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
    else if (current_process.status == "RUNNING" && gotta_preempt(current_process, t_slice))
    {
      // cout << "tf going on" << endl;
      output += "time " + to_string(curr_time) + "ms: Time slice expired;";
      if (!ready_queue.empty())
      {
        stats.num_preemptions++;
        if(current_process.cpu_bound) stats.cpu_num_preemptions++;
        else stats.io_num_preemptions++; 
        
        current_process.status = "PREEMPTED";
        current_process.switch_time = curr_time + t_cs / 2;
        output += " preempting process ";
        output += current_process.id;
        output += " with ";
        output += to_string(current_process.cpu_current_burst_remaining_time);
        output += "ms remaining ";
      }
      else
      {
        output += " no preemption because ready queue is empty ";
      }
      std::string o = print_queue(ready_queue);
      output += o + "\n";
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
        current_process.priority = 100;
        ready_queue.push(current_process);
        stats.entry_times[current_process.id] = curr_time; 
      }
    }

    // PROCESS STARTS USING CPU
    if (current_process.status == "SWITCH_IN")
    {
      // cout << current_process.id << " " << current_process.switch_time << " " << curr_time << endl;
      if (curr_time >= current_process.switch_time)
      {
        stats.context_switches++;
        if(current_process.cpu_bound) stats.cpu_context_switches++;
        else stats.io_context_switches++;

        stats.update_wait_time(current_process.id, curr_time - t_cs / 2, current_process.cpu_bound);
        current_process.status = "RUNNING";
        // issue here, if process was preempted, it needs to end at the remaining time
        if (current_process.was_preempted)
        {
          current_process.cpu_current_burst_finish_time = curr_time + current_process.cpu_current_burst_remaining_time;
          current_process.was_preempted = false;
          output += "time " + to_string(curr_time) + "ms: Process " + current_process.id + " started using the CPU for remaining ";
          output += to_string(current_process.cpu_current_burst_remaining_time) + "ms of " + to_string(current_process.cpu_bursts[current_process.current_burst_index]) + "ms burst ";
          output += print_queue(ready_queue) + "\n";
        }
        else
        {
          current_process.cpu_current_burst_finish_time = curr_time + current_process.cpu_bursts[current_process.current_burst_index];
          output += "time " + to_string(curr_time) + "ms: Process " + current_process.id + " started using the CPU for ";
          output += to_string(current_process.cpu_bursts[current_process.current_burst_index]) + "ms burst ";
          output += print_queue(ready_queue) + "\n";
        }
      }
    }

    // I/O BURST COMPLETION
    while (!waiting_queue.empty() && waiting_queue.top().io_current_burst_finish_time <= curr_time)
    {
      Process temp = waiting_queue.top();
      temp.status = "READY";
      temp.arrival_time = curr_time;
      temp.priority = 10;
      temp.current_burst_index = temp.current_burst_index + 1;
      temp.cpu_current_burst_remaining_time = temp.cpu_bursts[temp.current_burst_index];
      // temp.cpu_current_burst_finish_time = curr_time + temp.cpu_bursts[temp.current_burst_index];
      ready_queue.push(temp);
      stats.entry_times[temp.id] = curr_time; 
      waiting_queue.pop();
      output += "time " + to_string(curr_time) + "ms: Process " + temp.id + " completed I/O; added to ready queue ";
      output += print_queue(ready_queue) + "\n";
    }

    // NEW PROCESS ARRIVALS
    // if (curr_time <= 286)
    // {
    //   cout << current_process.id << " " <<current_process.status << " " << current_process.switch_time << " " << curr_time << endl;
    // }
    while (!arrival_queue.empty() && arrival_queue.top().arrival_time <= curr_time)
    {
      Process temp = arrival_queue.top();
      temp.status = "READY";
      // temp.cpu_current_burst_finish_time = curr_time + temp.cpu_bursts[temp.current_burst_index];
      temp.arrival_time = curr_time;
      temp.priority = 10;
      ready_queue.push(temp);
      stats.entry_times[temp.id] = curr_time; 
      arrival_queue.pop();
      output += "time " + to_string(curr_time) + "ms: Process " + temp.id + " arrived; added to ready queue ";
      output += print_queue(ready_queue) + "\n";
    }

    // if (curr_time >= 285 && curr_time <= 287)
    // {
    //   cout << current_process.id << " " <<current_process.status << " " << current_process.switch_time << " " << curr_time << endl;
    //   //print ready queue
    //   priority_queue<Process, vector<Process>, ReadyComparator> temp_ready_queue = ready_queue;
    //   while (!temp_ready_queue.empty())
    //   {
    //     Process temp = temp_ready_queue.top();
    //     cout << temp.id << " " << temp.arrival_time << " ";
    //     temp_ready_queue.pop();
    //   }
    //   cout << endl;
    // }
    // check if current process is not running
    // infinite loop here bc ts is somehow never running
    // if (curr_time >= 286 && curr_time <= 300)
    // {
    //   cout << current_process.id << " " <<current_process.status << " " << current_process.switch_time << " " << curr_time << endl;
    // }
    if (!ready_queue.empty() && ((current_process.status == "WAITING" || (current_process.status == "TERMINATED" && curr_time >= current_process.switch_time) || (current_process.status == "SWITCH_OUT" && curr_time >= current_process.switch_time) || (current_process.status == "PREEMPTED" && curr_time >= current_process.switch_time)) || (current_process.status == "READY" && current_process.was_preempted)))
    {
      // cout << current_process.id << " " << current_process.switch_time << " " << curr_time << endl;
      Process temp = ready_queue.top();
      temp.status = "SWITCH_IN";
      temp.switch_time = curr_time + t_cs / 2;
      ready_queue.pop();
      current_process = temp;
    }

    if (current_process.status == "RUNNING")
    {
      current_process.cpu_current_burst_remaining_time--;
      // stats.cpu_used_time++; 
    }

    if (curr_time < 10000)
    {
      cout << output;
    }

    curr_time++;
  }

  stats.total_time = curr_time; 
}
