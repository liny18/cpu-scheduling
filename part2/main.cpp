#include "FCFS.h"
#include "RR.h"
// #include "SJF.h"
// #include "SRT.h"
#include "Process.h"
#include <cmath>
#include <iostream>

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
      std::cout << "I/O-bound process ";
    }
    else
    {
      std::cout << "CPU-bound process ";
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

  std::cout << "<<< PROJECT PART I -- process set (n=" << n << ") with " << n_cpu << " CPU-bound " << (n_cpu == 1 ? "process >>>" : "processes >>>") << std::endl;

  print_processes(processes, cpu_bound_begin);
  cout << endl;

  std::cout << "<<< PROJECT PART II -- t_cs=" << t_cs << "ms; alpha=" << alpha << "; t_slice=" << t_slice << "ms >>>" << std::endl;

  // PART 2 START
  // run_fcfs(processes, t_cs);
  // cout << endl;

  run_rr(processes, t_cs, t_slice);

  return 0;
}
