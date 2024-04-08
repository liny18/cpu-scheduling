#include "FCFS.h"
#include "RR.h"
#include "SJF.h"
#include "SRT.h"
#include "Process.h"
#include "util.h"
#include <cmath>
#include <iostream>
#include <iomanip>


float next_exp(float lambda, int upper_bound)
{
  float exp = upper_bound + 1;
  while (exp > upper_bound)
  {
    exp = -log(drand48()) / lambda;
  }
  return exp;
}

void generate_processes(int n, int upper_bound, int cpu_bound_begin,
                        float lambda,
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
        id, arrival_time, cpu_burst_count, cpu_bursts, io_bursts, ceil(1 / lambda)));
  }
}

void print_processes(std::vector<Process> &processes,
                     int cpu_bound_begin)
{
  int count = 0;
  for (auto &process : processes)
  {
    if (count < cpu_bound_begin)
    {
      std::cout << "I/O-bound process ";
      process.cpu_bound = false;
    }
    else
    {
      std::cout << "CPU-bound process ";
      process.cpu_bound = true; 
    }
    std::cout << process.id << ": arrival time " << process.arrival_time << "ms; " << process.cpu_burst_count << " CPU bursts" << std::endl;
    count++;
  }
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
  float lambda = std::stod(argv[4]);
  int upper_bound = std::stoi(argv[5]);
  int t_cs = std::stoi(argv[6]);
  float alpha = std::stof(argv[7]);
  int t_slice = std::stoi(argv[8]);

  // the index of the first CPU-bound process
  int cpu_bound_begin = n - n_cpu;

  srand48(seed);

  std::vector<Process> processes;
  generate_processes(n, upper_bound, cpu_bound_begin, lambda, processes);

  std::cout << "<<< PROJECT PART I -- process set (n=" << n << ") with " << n_cpu << " CPU-bound " << (n_cpu == 1 ? "process >>>" : "processes >>>") << std::endl;

  print_processes(processes, cpu_bound_begin);
  cout << endl;

  std::cout << "<<< PROJECT PART II -- t_cs=" << t_cs << "ms; alpha=" << fixed << setprecision(2) << alpha << "; t_slice=" << t_slice << "ms >>>" << std::endl;

  // PART 2 START

  StatisticsHelper fcfs_stats; 
  StatisticsHelper sjf_stats; 
  StatisticsHelper srt_stats; 
  StatisticsHelper rr_stats; 

  run_fcfs(processes, t_cs, fcfs_stats);
  cout << endl;

  run_sjf(processes, t_cs, alpha, lambda, sjf_stats);
  cout << endl;

  run_srt(processes, t_cs, alpha, lambda, srt_stats);
  cout << endl;

  run_rr(processes, t_cs, t_slice, rr_stats);

  double num = ((double) fcfs_stats.total_cpu_burst_time / (double) fcfs_stats.total_time);
  double avg = ceil(num * 1000.0) / 1000.0;
  cout << fixed << setprecision(10) <<  avg << endl;
  // float avgBurstTime = ceil((totalBurstTime * 1000.0) / totalBursts) / 1000.0;

  // cout << "CPU UTILIZATION" << endl;
  // cout << fixed << setprecision(3) << (double) ((double) fcfs_stats.total_cpu_burst_time / (double) fcfs_stats.total_time) * 100.0 << endl;
  // cout << fixed << setprecision(3) << (double) ((double) sjf_stats.total_cpu_burst_time / (double) sjf_stats.total_time) * 100.0 << endl;
  // cout << fixed << setprecision(3) << (double) ((double) srt_stats.total_cpu_burst_time / (double) srt_stats.total_time) * 100.0 << endl;
  // cout << fixed << setprecision(3) << (double) ((double) rr_stats.total_cpu_burst_time / (double) rr_stats.total_time) * 100.0 << endl;

  // cout << "CPU BURST TIME VALUES" << endl;
  // cout << fixed << setprecision(3) << ((double) fcfs_stats.total_cpu_burst_time / (double) fcfs_stats.total_cpu_bursts) << endl;
  // cout << fixed << setprecision(3) << (double) sjf_stats.total_cpu_burst_time / (double) sjf_stats.total_cpu_bursts << endl;
  // cout << fixed << setprecision(3) << (double) srt_stats.total_cpu_burst_time / (double) srt_stats.total_cpu_bursts << endl;
  // cout << fixed << setprecision(3) << (double) rr_stats.total_cpu_burst_time / (double) rr_stats.total_cpu_bursts << endl;

  // cout << fixed << setprecision(3) << ((double) fcfs_stats.cpu_bound_burst_time / (double) fcfs_stats.cpu_bound_bursts) << endl;
  // cout << fixed << setprecision(3) << ((double) sjf_stats.cpu_bound_burst_time / (double) sjf_stats.cpu_bound_bursts) << endl;
  // cout << fixed << setprecision(3) << ((double) srt_stats.cpu_bound_burst_time / (double) srt_stats.cpu_bound_bursts) << endl;
  // cout << fixed << setprecision(3) << ((double) rr_stats.cpu_bound_burst_time / (double) rr_stats.cpu_bound_bursts) << endl;

  // cout << fixed << setprecision(3) << ((double) fcfs_stats.io_bound_burst_time / (double) fcfs_stats.io_bound_bursts) << endl;
  // cout << fixed << setprecision(3) << ((double) sjf_stats.io_bound_burst_time / (double) sjf_stats.io_bound_bursts) << endl;
  // cout << fixed << setprecision(3) << ((double) srt_stats.io_bound_burst_time / (double) srt_stats.io_bound_bursts) << endl;
  // cout << fixed << setprecision(3) << ((double) rr_stats.io_bound_burst_time / (double) rr_stats.io_bound_bursts) << endl;

  // cout << "WAIT TIME VALUES" << endl;
  // cout << fixed << setprecision(3) << ((double) fcfs_stats.total_wait_time / (double) fcfs_stats.total_cpu_bursts) << endl;
  // cout << fixed << setprecision(3) << ((double) sjf_stats.total_wait_time / (double) sjf_stats.total_cpu_bursts) << endl;
  // cout << fixed << setprecision(3) << ((double) srt_stats.total_wait_time / (double) srt_stats.total_cpu_bursts) << endl;
  // cout << fixed << setprecision(3) << ((double) rr_stats.total_wait_time / (double) rr_stats.total_cpu_bursts) << endl;

  // cout << fixed << setprecision(3) << ((double) fcfs_stats.cpu_bound_wait_time / (double) fcfs_stats.cpu_bound_bursts) << endl;
  // cout << fixed << setprecision(3) << ((double) sjf_stats.cpu_bound_wait_time / (double) sjf_stats.cpu_bound_bursts) << endl;
  // cout << fixed << setprecision(3) << ((double) srt_stats.cpu_bound_wait_time / (double) srt_stats.cpu_bound_bursts) << endl;
  // cout << fixed << setprecision(3) << ((double) rr_stats.cpu_bound_wait_time / (double) rr_stats.cpu_bound_bursts) << endl;

  // cout << fixed << setprecision(3) << ((double) fcfs_stats.io_bound_wait_time / (double) fcfs_stats.io_bound_bursts) << endl;
  // cout << fixed << setprecision(3) << ((double) sjf_stats.io_bound_wait_time / (double) sjf_stats.io_bound_bursts) << endl;
  // cout << fixed << setprecision(3) << ((double) srt_stats.io_bound_wait_time / (double) srt_stats.io_bound_bursts) << endl;
  // cout << fixed << setprecision(3) << ((double) rr_stats.io_bound_wait_time / (double) rr_stats.io_bound_bursts) << endl;

  // cout << "TURNAROUND TIME STATS" << endl;

  // cout << "CONTEXT SWITCH TRACKER" << endl;
  // cout << fcfs_stats.context_switches << " " << fcfs_stats.cpu_context_switches <<   " " << fcfs_stats.io_context_switches<< endl;
  // cout << sjf_stats.context_switches << " " << sjf_stats.cpu_context_switches <<   " " << sjf_stats.io_context_switches<< endl;
  // cout << srt_stats.context_switches << " " << srt_stats.cpu_context_switches <<   " " << srt_stats.io_context_switches<< endl;
  // cout << rr_stats.context_switches << " " << rr_stats.cpu_context_switches <<   " " << rr_stats.io_context_switches<< endl;

  // cout << "PREEMPTION TRACKER" << endl;
  // cout << fcfs_stats.num_preemptions << " " << fcfs_stats.cpu_num_preemptions <<   " " << fcfs_stats.io_num_preemptions<< endl;
  // cout << sjf_stats.num_preemptions << " " << sjf_stats.cpu_num_preemptions <<   " " << sjf_stats.io_num_preemptions<< endl;
  // cout << srt_stats.num_preemptions << " " << srt_stats.cpu_num_preemptions <<   " " << srt_stats.io_num_preemptions<< endl;
  // cout << rr_stats.num_preemptions << " " << rr_stats.cpu_num_preemptions <<   " " << rr_stats.io_num_preemptions<< endl;
  return 0;
}
