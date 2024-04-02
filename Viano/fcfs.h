#ifndef __FCFS__
#define __FCFS__

#include <limits.h>
#include "process.h"
#include "queue.h"
#include "bool.h"
#include "algorithm_helper.h"
#include "cpu.h"
#include "print.h"

// Forward Declaration
void FCFSAlgorithm(Process* processes, int total_processes, int context_switch);
void FCFSCPUProcessBegin(Queue* queue, CPU* cpu, int time);
void FCFSCPUBurstComplete(Queue* queue, CPU* cpu, int time, int context_switch, int* termination);
void FCFSIOBurstCompletion(Process* processes, Queue* queue, CPU* cpu, int time, int context_switch, int total_processes);

// Begin to process in the CPU
void FCFSCPUProcessBegin(Queue* queue, CPU* cpu, int time){
    if(cpu->state == OCCUPIED && cpu->p->state == CONTEXT_SWITCH_IN && cpu->p->current_wait_time == 0){
        StartBurst(queue, cpu->p, cpu, time);
    }
}

/* Process a CPU Burst after it has finished
    - A CPU burst can be terminated
        remove the process from the CPU entirely
    - A CPU burst can finish normally
        - the CPU burst has finished all of it's bursts
            terminate the process and enter context switch
        - the CPU burst has finished one of it's bursts (not the last)
            end the burst
            begin I/O waiting
*/
void FCFSCPUBurstComplete(Queue* queue, CPU* cpu, int time, int context_switch, int* termination){
    if(cpu->state == OCCUPIED && cpu->p->current_wait_time == 0){
        switch(cpu->p->state){
            case TERMINATED:
                cpu->state = NONE;
                cpu->p = NULL;
                break;
            case RUNNING:
                if(cpu->p->current_burst == cpu->p->total_bursts - 1){ // CPU has been terminated and needs to be removed
                    Terminate(queue, cpu, time);
                    *termination += 1;
                    ContextSwitch(cpu, cpu->p, context_switch);
                } else {
                    EndBurst(queue, cpu, time);
                    BeginIO(queue, cpu, time, context_switch);
                }
                break;
            default:
                break;
        }
    }
}

// For each process determine if an IO process is finished, and then complete their bursts
void FCFSIOBurstCompletion(Process* processes, Queue* queue, CPU* cpu, int time, int context_switch, int total_processes){
    for(int x = 0; x < total_processes; x++){
        // if this process is in the CPU, then it is not waiting for I/O
        if((cpu->state == NONE || cpu->p->name != processes[x].name) &&
        processes[x].state == WAITING && processes[x].current_wait_time == 0){
            IOBurst(processes+x, queue, time);
        }
    }
}

// Main algorithm for FCFS
void FCFSAlgorithm(Process* processes, int total_processes, int context_switch){
    // ===================== DECLARATION/INITIALIZATION =====================
    unsigned int time = 0;
    double util = 0;
    double wait_time = 0;
    int ct_switch = 0;
    Queue queue;
    CPU cpu;
    initializeQueue(&queue);
    initializeCPU(&cpu);
    int termination_check = 0;
    // ===================== BEGIN SIMULATION =====================
    printf("time %dms: Simulator started for FCFS ", time);
    printQueue(&queue);
    printf("\n");
    while(time < UINT_MAX){
        // ===================== CPU BURST COMPLETION =====================
        FCFSCPUBurstComplete(&queue, &cpu, time, context_switch, &termination_check);
        // ===================== CPU PROCESS BEGIN =====================
        FCFSCPUProcessBegin(&queue, &cpu, time);
        // ===================== I/O BURST COMPLETION =====================
        RemoveProcessFromCPU(&queue, &cpu);
        FCFSIOBurstCompletion(processes, &queue, &cpu, time, context_switch, total_processes);
        // ===================== PROCESS ARRIVAL =====================
        Arrival(processes, &queue, &cpu, time, total_processes, context_switch);
        ct_switch += InsertProcessToCPU(&queue, &cpu, context_switch);

        ProcessWaitTime(&queue, &cpu, processes, total_processes);
        if(termination_check == total_processes)
            break;

        if(cpu.state == OCCUPIED && cpu.p->state == RUNNING)
            util += 1;
        for(int i = 0;i < total_processes; i++) {
            if(processes[i].state == READY)
                wait_time += 1;
        }
        
        time += 1;
    }
    time += context_switch / 2;
    printf("time %dms: Simulator ended for FCFS ", time);
    print(0, "FCFS", processes, wait_time / ct_switch, ct_switch, 0, util / time, total_processes, wait_time, context_switch);
    printQueue(&queue);
    printf("\n");
    freeQueue(&queue);
}

#endif