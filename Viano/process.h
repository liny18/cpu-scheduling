#ifndef __PROCESS__
#define __PROCESS__

typedef enum {
  RUNNING, READY, WAITING, ENTRY, TERMINATED, CONTEXT_SWITCH_IN, CONTEXT_SWITCH_OUT
} State;

typedef enum {
  DEFAULT, STALLING
} PreemptiveState;

typedef struct process {
  char name;
  int total_bursts;
  int* cpu_burst_times;
  int* io_burst_times;
  int arrival_time;
  int current_burst;
  int current_io;
  int current_wait_time;
  int estimated_wait_time;
  int save_wait_time;
  int tau;
  State state;
  PreemptiveState p_state;
} Process;

#endif