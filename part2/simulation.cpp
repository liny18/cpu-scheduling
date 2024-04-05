#include "Process.h"
#include <cmath>
#include <iostream>
#include <queue>

using namespace std;

double next_exp(double lambda, int upper_bound)
{
  double exp = upper_bound + 1;
  while (exp > upper_bound)
  {
    exp = -log(drand48()) / lambda;
  }
  return exp;
}

void generate_processes(int n, int upper_bound, int cpu_bound_begin,
                        double lambda,
                        std::vector<Process> &processes)
{
  for (int i = 0; i < n; i++)
  {
    char id = 'A' + i;
    int arrival_time = floor(next_exp(lambda, upper_bound));
    int cpu_burst_count = 0;
    cpu_burst_count = ceil(drand48() * 64);
    std::vector<int> cpu_bursts;
    std::vector<int> io_bursts;

    for (int j = 0; j < cpu_burst_count; j++)
    {
      int burst_time = ceil(next_exp(lambda, upper_bound));
      burst_time *= (i >= cpu_bound_begin) ? 4 : 1;

      cpu_bursts.push_back(burst_time);

      if (j < cpu_burst_count - 1)
      {
        int io_time = ceil(next_exp(lambda, upper_bound)) * 10;
        io_time /= (i >= cpu_bound_begin) ? 8 : 1;
        io_bursts.push_back(io_time);
      }
    }

    processes.push_back(Process(
        id, arrival_time, cpu_burst_count, cpu_bursts, io_bursts));
  }
}

void print_processes(const std::vector<Process> &processes,
                     int cpu_bound_begin)
{
  int count = 0;
  for (const auto &process : processes)
  {
    if (count < cpu_bound_begin)
    {
      std::cout << "ms: I/O-bound process ";
    }
    else
    {
      std::cout << "ms: CPU-bound process ";
    }
    std::cout << process.id << ": arrival time " << process.arrival_time << "ms; " << process.cpu_burst_count << " CPU bursts" << std::endl;
    count++;
  }
}

std::string print_queue(const std::deque<Process> &queue)
{
  if (queue.empty())
  {
    return "[Q <empty>]";
  }

  std::string output = "[Q";
  for (const auto &process : queue)
  {
    std::string temp = " ";
    temp += process.id;
    output += temp;
  }
  output += "]";
  return output;
}

int main(int argc, char *argv[])
{
  if (argc != 9)
  {
    std::cerr << "ERROR: Invalid number of arguments" << std::endl;
    return 1;
  }

  int n = std::stoi(argv[1]);
  int n_cpu = std::stoi(argv[2]);
  int seed = std::stoi(argv[3]);
  double lambda = std::stod(argv[4]);
  int upper_bound = std::stoi(argv[5]);
  int t_cs = std::stoi(argv[6]);
  float alpha = std::stof(argv[7]);
  int t_slice = std::stoi(argv[8]);

  // the index of the first CPU-bound process
  int cpu_bound_begin = n - n_cpu;

  srand48(seed);

  std::vector<Process> processes;
  generate_processes(n, upper_bound, cpu_bound_begin, lambda, processes);

  // std::cout << "<<< PROJECT PART I -- process set (n=" << n << ") with " << n_cpu << " CPU-bound " << (n_cpu == 1 ? "process >>>" : "processes >>>") << std::endl;

  // print_processes(processes, cpu_bound_begin);

  std::cout << "<<< PROJECT PART II -- t_cs=" << t_cs << "ms; alpha=" << alpha << "; t_slice=" << t_slice << "ms >>>" << std::endl;

  // PART 2 START : FCFS
  auto arrival_cmp = [](Process p1, Process p2)
  { return p1.arrival_time > p2.arrival_time; };

  std::priority_queue<Process, vector<Process>, decltype(arrival_cmp)> arrival_queue(arrival_cmp);
  for (auto p : processes)
    arrival_queue.push(p);
  std::deque<Process> ready_queue;
  std::deque<Process> waiting_queue;

  int curr_time = 0;
  Process current_process = Process();

  cout << "time " << curr_time << "ms: Simulator started for FCFS [Q <empty>]" << endl;

  while (true)
  {
    // cout << "time " << curr_time << " " << arrival_queue.size() << " " << ready_queue.size() << " " << waiting_queue.size() << endl;
    // terminate when to complete
    //(a) CPU burst completion; (b) process starts using the CPU; (c) I/O burst completions; and (d) new process arrivals
    std::string output = "";
    if (arrival_queue.empty() && ready_queue.empty() && waiting_queue.empty() && current_process.status == "TERMINATED")
      break;
    // CPU BURST COMPLETION:
    if (current_process.status == "RUNNING" && curr_time >= current_process.cpu_current_burst_finish_time)
    {
      if (current_process.current_burst_index == current_process.cpu_burst_count - 1)
      {
        // output += "time " + to_string(curr_time) + "ms: PROCESS " + current_process.id + " terminated\n";
        current_process.status = "TERMINATED";
        cout << "time " << curr_time << "ms: PROCESS " << current_process.id << " terminated";
        std::string o = print_queue(ready_queue);
        cout << o << endl;
        // NEED TO FIGURE OUT HOW TO ACTUAL TERMINATE
      }
      else
      {
        current_process.status = "SWITCH_OUT";
        current_process.switch_time = curr_time + t_cs / 2;
        output += "time " + to_string(curr_time) + "ms: PROCESS " + current_process.id + " completed a CPU burst;";
        output += print_queue(ready_queue) + "\n";
        output += "time " + to_string(curr_time) + "ms: PROCESS " + current_process.id + " switching out of CPU; blocking on I/O ";
        output += print_queue(ready_queue) + "\n";
        // current_process = Process();
      }
    }
    // DOOM!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    else if (current_process.status == "SWITCH_OUT")
    {
      if (curr_time >= current_process.switch_time)
      {
        current_process.status = "WAITING";
        current_process.io_current_burst_finish_time = curr_time + current_process.io_bursts[current_process.current_burst_index];
        waiting_queue.push_back(current_process);
        // current_process = Process();
      }
    }
    // if (curr_time == current_process.cpu_current_burst_finish_time)
    // {
    //   if (current_process.current_burst_index == current_process.cpu_burst_count - 1)
    //   {
    //     current_process.status = "TERMINATED";
    //     output += "time " + to_string(curr_time) + "ms: PROCESS " + current_process.id + " terminated\n";
    //     // NEED TO FIGURE OUT HOW TO ACTUAL TERMINATE
    //   }
    //   else
    //   {
    //     current_process.status = "WAITING";
    //     int add_time = curr_time;
    //     if (!ready_queue.empty())
    //     {
    //       add_time += ready_queue.back().cpu_current_burst_finish_time;
    //     }
    //     current_process.io_current_burst_finish_time = add_time + current_process.io_bursts[current_process.current_burst_index];
    //     waiting_queue.push_back(current_process);
    //     output += "time " + to_string(curr_time) + "ms: PROCESS " + current_process.id + " completed a CPU burst;";
    //     output += print_queue(ready_queue) + "\n";
    //     output += "time " + to_string(curr_time) + "ms: PROCESS " + current_process.id + " switching out of CPU; blocking on I/O ";
    //     output += print_queue(ready_queue) + "\n";
    //     current_process = Process();
    //   }
    // }

    // PROCESS STARTS USING CPU
    if (current_process.status == "SWITCH_IN")
    {
      if (curr_time >= current_process.switch_time)
      {
        current_process.status = "RUNNING";
        current_process.cpu_current_burst_finish_time = curr_time + current_process.cpu_bursts[current_process.current_burst_index];
        output += "time " + to_string(curr_time) + "ms: PROCESS " + current_process.id + " started using the CPU for ";
        output += to_string(current_process.cpu_bursts[current_process.current_burst_index]) + "ms burst ";
        output += print_queue(ready_queue) + "\n";
      }
    }
    // if (current_process.id == -1 || current_process.status == "TERMINATED")
    // {
    //   if (!ready_queue.empty())
    //   {
    //     current_process = ready_queue.front();
    //     current_process.cpu_current_burst_finish_time = curr_time + current_process.cpu_bursts[current_process.current_burst_index];
    //     current_process.status = "RUNNING";
    //     ready_queue.pop_front();
    //     output += "time " + to_string(curr_time) + "ms: PROCESS " + current_process.id + " started using the CPU for ";
    //     output += to_string(current_process.cpu_bursts[current_process.current_burst_index]) + "ms burst ";
    //     output += print_queue(ready_queue) + "\n";
    //   }
    // }

    // IO BURST COMPLETION
    while (!waiting_queue.empty() && waiting_queue.front().io_current_burst_finish_time <= curr_time)
    {
      Process temp = waiting_queue.front();
      temp.status = "READY";
      temp.current_burst_index = temp.current_burst_index + 1;
      int add_time = curr_time;
      if (!ready_queue.empty())
      {
        add_time += ready_queue.back().cpu_current_burst_finish_time;
      }
      temp.cpu_current_burst_finish_time = add_time + temp.cpu_bursts[temp.current_burst_index];
      ready_queue.push_back(temp);
      waiting_queue.pop_front();
      output += "time " + to_string(curr_time) + "ms: PROCESS " + temp.id + " completed I/O; added to ready queue ";
      output += print_queue(ready_queue) + "\n";
    }

    // NEW PROCESS ARRIVALS
    while (!arrival_queue.empty() && arrival_queue.top().arrival_time <= curr_time)
    {
      Process temp = arrival_queue.top();
      temp.status = "READY";
      // temp.cpu_current_burst_finish_time = curr_time + temp.cpu_bursts[temp.current_burst_index];
      ready_queue.push_back(temp);
      arrival_queue.pop();
      output += "time " + to_string(curr_time) + "ms: PROCESS " + temp.id + " arrived; added to ready queue ";
      output += print_queue(ready_queue) + "\n";
    }

    // check if current process is not running
    if (!ready_queue.empty() && (current_process.status == "WAITING" || current_process.status == "TERMINATED" || (current_process.status == "SWITCH_OUT" && curr_time >= current_process.switch_time)))
    {
      Process temp = ready_queue.front();
      temp.status = "SWITCH_IN";
      temp.switch_time = curr_time + t_cs / 2;
      ready_queue.pop_front();
      current_process = temp;
    }

    if (curr_time < 10000)
    {
      cout << output;
    }
    curr_time++;
  }

  cout << "time " << curr_time << "ms: Simulator ended for FCFS [Q <empty>]" << endl;

  return 0;
}
