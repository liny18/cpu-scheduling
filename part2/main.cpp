#include "FCFS.h"
#include "RR.h"
#include "SJF.h"
#include "SRT.h"
#include "Process.h"
#include "util.h"
#include <cmath>
#include <iostream>
#include <iomanip>
#include <map>
#include <fstream>


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


  map<string, StatisticsHelper> mp;
  mp["aFCFS"] = fcfs_stats; 
  mp["bSJF"] = sjf_stats; 
  mp["cSRT"] = srt_stats; 
  mp["dRR"] = rr_stats;

  ofstream myfile;
  myfile.open("simout.txt");


  for(auto &[k,v] : mp) {
    string name = k.substr(1,k.size());
    double cpu_utilization = ceil((v.total_cpu_burst_time * 1000.0) / v.total_time * 1000.0) / 10000.0;
    double avg_total_burst_time = ceil((v.total_cpu_burst_time * 1000.0) / v.total_cpu_bursts) / 1000.0;
    double avg_cpu_burst_time = ceil((v.cpu_bound_burst_time * 1000.0) / v.cpu_bound_bursts) / 1000.0;
    double avg_io_burst_time = ceil((v.io_bound_burst_time * 1000.0) / v.io_bound_bursts) / 1000.0;
    double avg_total_wait_time = ceil((v.total_wait_time * 1000.0) / v.total_cpu_bursts) / 1000.0;
    double avg_cpu_wait_time = ceil((v.cpu_bound_wait_time * 1000.0) / v.cpu_bound_bursts) / 1000.0;
    double avg_io_wait_time = ceil((v.io_bound_wait_time * 1000.0) / v.io_bound_bursts) / 1000.0;
    double avg_turnaround_total = (double)((double)v.total_cpu_burst_time / (double)v.total_cpu_bursts) + (double)((double)(v.total_wait_time + v.num_preemptions * t_cs) / (double)(v.cpu_bound_bursts + v.io_bound_bursts)) + t_cs;
    avg_turnaround_total *= 1000.0;
    avg_turnaround_total = ceil(avg_turnaround_total);
    avg_turnaround_total /= 1000.0;
    double avg_turnaround_cpu = (double)((double)v.cpu_bound_burst_time / (double)v.cpu_bound_bursts) + (double)((double)(v.cpu_bound_wait_time + v.cpu_num_preemptions * t_cs) / (double)(v.cpu_bound_bursts)) + t_cs;
    avg_turnaround_cpu *= 1000.0;
    avg_turnaround_cpu = ceil(avg_turnaround_cpu);
    avg_turnaround_cpu /= 1000.0;
    double avg_turnaround_io = (double)((double)v.io_bound_burst_time / (double)v.io_bound_bursts) + (double)((double)(v.io_bound_wait_time + v.io_num_preemptions * t_cs) / (double)(v.io_bound_bursts)) + t_cs;
    avg_turnaround_io *= 1000.0;
    avg_turnaround_io = ceil(avg_turnaround_io);
    avg_turnaround_io /= 1000.0;

    myfile << "Algorithm " << name << "\n";
    myfile << "-- CPU utilization: " << fixed << setprecision(3) << cpu_utilization << "%\n";
    myfile << "-- average CPU burst time: " << avg_total_burst_time << " ms (" << avg_cpu_burst_time << " ms/" << avg_io_burst_time << " ms)"  << "\n";
    myfile << "-- average wait time: " << avg_total_wait_time << " ms (" << avg_cpu_wait_time << " ms/"<<avg_io_wait_time << " ms)" <<  "\n";
    myfile << "-- average turnaround time: " << avg_turnaround_total << " ms (" << avg_turnaround_cpu << " ms/"<<avg_turnaround_io << " ms)" <<  "\n";
    myfile << "-- number of context switches: " << v.context_switches << " (" << v.cpu_context_switches << "/" << v.io_context_switches << ")"<<"\n";
    if(name != "RR") myfile << "-- number of preemptions: "<< v.num_preemptions << " (" << v.cpu_num_preemptions << "/" << v.io_num_preemptions << ")"<<"\n\n";
    else myfile << "-- number of preemptions: "<< v.num_preemptions << " (" << v.cpu_num_preemptions << "/" << v.io_num_preemptions << ")"<<"\n";
  }

  myfile.close();

  return 0;
}
