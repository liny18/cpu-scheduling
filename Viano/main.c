/* CPU Scheduling Simulator
 *   Tyler Chan, Zhi Zheng, Viano Arcery
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "process.h"
#include "queue.h"
#include "bool.h"
// Algorithms
#include "fcfs.h"
#include "sjf.h"
#include "srt.h"
#include "rr.h"

char process_names[26] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'};

double nextExp(double lambda, double upper_bound);
void generateProcess(Process* processes, int total_processes, double lambda, double upper_bound);
void debugProcess(Process p);
void printProcess(Process p);
void printAllProcesses(Process* processes, int size);
void freeAllProcesses(Process* processes, int size);

int main(int argc, char * argv[]) {

  setvbuf(stdout, NULL, _IONBF, 0);

  // ===================== INPUTS =====================
  if (argc < 8) {
    fprintf(stderr, "Usage: ./main <n> <seed> <lambda> <upper-bound> <time for context switch> <alpha> <time slice>\n");
    return 1;
  }

  /* Check if n is > 0 && < 26 */
  int n = atoi(argv[1]);
  if (n < 1 || n > 26) {
    fprintf(stderr, "ERROR: The number of processes must be > 0 and <= 26 : %d\n", n);
    return 1;
  }

  long int seed = atol(argv[2]);
  srand48(seed);

  /* next_exp() returns inf if lambda is 0 */
  double lambda = atof(argv[3]);
  if (lambda == (double) 0) {
    fprintf(stderr, "ERROR: lambda cannot be 0.\n");
  }

  double upper_bound = atof(argv[4]);
  if (upper_bound == 0.0) {
    fprintf(stderr, "ERROR: upper-bound cannot be 0.\n");
  }

  // context switch time
  int t_context_switch = atoi(argv[5]);
  if (t_context_switch % 2 != 0 || t_context_switch <= 0) {
    fprintf(stderr, "ERROR: The context switch time is expected to be a positive even integer\n");
    return EXIT_FAILURE;
  }

  // alpha (SJF), (SRT)
  double alpha = atof(argv[6]);

  // tslice (RR)
  int t_slice = atoi(argv[7]);
  if(t_slice <= 0){
    fprintf(stderr, "ERROR: Time slice for Round Robin must be positive\n");
    return EXIT_FAILURE;
  }

  // ===================== GENERATE PROCESSES =====================
  Process* processes = calloc(n, sizeof(Process));
  generateProcess(processes, n, lambda, upper_bound);
  printAllProcesses(processes, n);
  printf("\n");
  // ===================== RUN ALGORITHMS =====================
  pid_t p1 = fork();
  if ( p1 == -1 ){
    fprintf(stderr, "ERROR: fork() failed\n");
    return EXIT_FAILURE;
  }
  if ( p1 == 0 ){
    FCFSAlgorithm(processes, n, t_context_switch);
    printf("\n");
  } else {
    waitpid(p1, NULL, 0); 
    pid_t p2 = fork();
    if ( p2 == -1 ){
      fprintf(stderr, "ERROR: fork() failed\n");
      return EXIT_FAILURE;
    }
    if( p2 == 0 ){
      SJFAlgorithm(processes, n, t_context_switch, alpha);
      printf("\n");
    } else {
      waitpid(p2, NULL, 0); 
      pid_t p3 = fork();
      if ( p3 == -1 ){
        fprintf(stderr, "ERROR: fork() failed\n");
        return EXIT_FAILURE;
      }
      if( p3 == 0 ){
        SRTAlgorithm(processes, n, t_context_switch, alpha);
        printf("\n");
      } else {
        waitpid(p3, NULL, 0); 
        RRAlgorithm(processes, n, t_context_switch, t_slice);
      }
    }
  }
  // ===================== MEMORY DEALLOCATION =====================
  freeAllProcesses(processes, n);
  return EXIT_SUCCESS;
}

void printProcess(Process p){
  if (p.total_bursts == 1) {
    printf("Process %c: arrival time %dms; tau %dms; %d CPU burst:\n", p.name, p.arrival_time, p.tau, p.total_bursts);
  } else {
    printf("Process %c: arrival time %dms; tau %dms; %d CPU bursts:\n", p.name, p.arrival_time, p.tau, p.total_bursts);
  }
  int count = 0;
  while(count < p.total_bursts){
    printf("--> CPU burst %dms", p.cpu_burst_times[count]);
    if(count < p.total_bursts-1)
      printf(" --> I/O burst %dms\n", p.io_burst_times[count]);
    count++;
  }
  printf("\n");
}

void debugProcess(Process p){
  printf("================= PROCESS %c =================\n", p.name);
  printf("Total Bursts: %dms\n", p.total_bursts);
  printf("Arrival Time: %dms\n", p.arrival_time);
  printf("=============================================\n");
  int count = 0;
  while(count < p.total_bursts){
    printf("--> CPU burst %dms", p.cpu_burst_times[count]);
    if(count < p.total_bursts-1)
      printf(" --> I/O burst %dms\n", p.io_burst_times[count]);
    count++;
  }
  printf("\n");
}

double nextExp(double lambda, double upper_bound) {
  double num = -log(drand48()) / lambda;
  while(num > upper_bound){
    num = -log(drand48()) / lambda;
  }
  return num;
}

void generateProcess(Process* processes, int total_processes, double lambda, double upper_bound) {
  int ctr = 0;
  int count = 0;
  while (ctr < total_processes) {
    Process p;
    p.name = process_names[ctr];
    p.arrival_time = floor(nextExp(lambda, upper_bound));
    p.total_bursts = ceil(drand48() * 100);
    p.cpu_burst_times = calloc(p.total_bursts, sizeof(int));
    p.io_burst_times = calloc(p.total_bursts-1, sizeof(int));
    p.current_burst = 0;
    p.current_io = 0;
    p.current_wait_time = 0;
    p.save_wait_time = 0;
    p.state = ENTRY;
    p.p_state = DEFAULT;
    count = 0;
    p.tau = 1/lambda;
    p.estimated_wait_time = p.tau;
    while(count < p.total_bursts){
      p.cpu_burst_times[count] = ceil(nextExp(lambda, upper_bound));
      if(count < p.total_bursts-1)
        p.io_burst_times[count] = ceil(nextExp(lambda, upper_bound)) * 10;
      count++;
    }
    processes[ctr] = p;
    ctr++;
  }
}

void printAllProcesses(Process* processes, int size){
  for(int x = 0; x < size; x++){
    printProcess(processes[x]);
  }
}

void freeAllProcesses(Process* processes, int size){
  for(int x = 0; x < size; x++){
    free(processes[x].cpu_burst_times);
    free(processes[x].io_burst_times);
  }
  free(processes);
}