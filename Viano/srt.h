#ifndef __SRT__
#define __SRT__

#include <limits.h>
#include "process.h"
#include "queue.h"
#include "bool.h"
#include "algorithm_helper.h"
#include "sjf.h"
#include "cpu.h"
#include "print.h"

// Forward Declaration
void SRTAlgorithm(Process* processes, int total_processes, int context_switch, double alpha);
void SRTCPUProcessBegin(Queue* queue, CPU* cpu, int time);
void SRTIOBurstCompletion(Process* processes, Queue* queue, CPU* cpu, int time, int context_switch, int total_processes, int* preem);
void BeginStallingSRT(Queue* queue, CPU* cpu);
void SRTRemoveProcessFromCPU(Queue* queue, CPU* cpu);
void SRTPreemption(Queue* queue, CPU* cpu, int time, int context_switch, int* preem);

// Begin to process in the CPU
void SRTCPUProcessBegin(Queue* queue, CPU* cpu, int time){
    if(cpu->state == OCCUPIED && cpu->p->state == CONTEXT_SWITCH_IN && cpu->p->current_wait_time == 0){
        StartPreemptiveBurstTAU(queue, cpu->p, cpu, time);
    }
}

// For each process determine if an IO process is finished, and complete their bursts
void SRTIOBurstCompletion(Process* processes, Queue* queue, CPU* cpu, int time, int context_switch, int total_processes, int* preem){
    for(int x = 0; x < total_processes; x++){
        // if this process is in the CPU, then it is not waiting for I/O
        if(cpu->state == OCCUPIED && cpu->p->name == processes[x].name)
            continue;
        if(processes[x].state == WAITING && processes[x].current_wait_time == 0){
            // compare process to CPU, if this process is shorter then we pre-emp it
            // NOTE: estimated wait time is already 1 second shorter than starting tau value so we compare it by 1 less
            if(cpu->state == OCCUPIED && cpu->p->state != CONTEXT_SWITCH_IN && cpu->p->state != CONTEXT_SWITCH_OUT
            && processes[x].tau < cpu->p->estimated_wait_time){
                cpu->p->save_wait_time = cpu->p->current_wait_time;
                cpu->p->current_wait_time = 0;
                ContextSwitch(cpu, cpu->p, context_switch);
                cpu->p->state = CONTEXT_SWITCH_OUT;
                cpu->p->p_state = STALLING;
                IOBurstPreemptiveTAU(cpu->p, processes+x, queue, time);
                *preem += 1;
            } else {
                IOBurstTAU(processes+x, queue, time);
            }
        }
    }
}

// Stall the process in the CPU
// NOTE: this version is with enqueueEstimated
void BeginStallingSRT(Queue* queue, CPU* cpu){
    cpu->p->current_wait_time = 0;
    cpu->p->state = READY;
    enqueueEstimated(queue, cpu->p);
    cpu->state = NONE;
    cpu->p = NULL;
}

/*  Remove a process from the CPU once context switch out is finished running
    Based on the processing state:
        - DEFAULT
            the process has finished and will move to I/O
        - STALLING
            the process has been removed for pre-emptive reasons, 
            and will begin stalling in the ready queue
*/
void SRTRemoveProcessFromCPU(Queue* queue, CPU* cpu){
    if(cpu->state == OCCUPIED && cpu->p->state == CONTEXT_SWITCH_OUT && cpu->p->current_wait_time == 0){
        switch(cpu->p->p_state){
            case DEFAULT:
                IOWaitDone(cpu);
                break;
            case STALLING:
                BeginStallingSRT(queue, cpu);
                break;
        }
    }
}

/*  Preemption for SRT
    No preemptions happen when the queue is empty or if the CPU has no process;
    When the CPU is RUNNING and the CPU process can be preempt, because the process's 
    estimated time (tau) is shorter than the CPU's process's estimated waiting time
        preempt the process and context switch out the CPU process to stall
*/
void SRTPreemption(Queue* queue, CPU* cpu, int time, int context_switch, int* preem){
    if(isEmpty(queue) || cpu->state == NONE)
        return;
    Process* process = queue->front->process;
    if(cpu->p->state == RUNNING && cpu->p->current_wait_time != 0 && 
    process->tau < cpu->p->estimated_wait_time && process->state == READY){
        if (ableToPrint(time)) {
            printf("time %dms: Process %c (tau %dms) will preempt %c ", time, process->name, process->tau, cpu->p->name); 
            printQueue(queue);
            printf("\n");
        }
        *preem += 1;
        // pause current process (stall)
        cpu->p->save_wait_time = cpu->p->current_wait_time;
        cpu->p->current_wait_time = 0;
        ContextSwitch(cpu, cpu->p, context_switch);
        cpu->p->state = CONTEXT_SWITCH_OUT;
        cpu->p->p_state = STALLING;
    }
}

// Main algorithm for SRT
void SRTAlgorithm(Process* processes, int total_processes, int context_switch, double alpha){
    // ===================== DECLARATION/INITIALIZATION =====================
    unsigned int time = 0;
    double util = 0;
    double wait_time = 0;
    int preem = 0;
    int ct_switch = 0;
    Queue queue;
    CPU cpu;
    initializeQueue(&queue);
    initializeCPU(&cpu);
    int termination_check = 0;
    // ===================== BEGIN SIMULATION =====================
    printf("time %dms: Simulator started for SRT ", time);
    printQueue(&queue);
    printf("\n");
    while(time < UINT_MAX){
        // ===================== CPU BURST COMPLETION =====================
        SJFCPUBurstComplete(&queue, &cpu, time, context_switch, &termination_check, alpha);
        // ===================== CPU PROCESS BEGIN =====================
        SRTCPUProcessBegin(&queue, &cpu, time);
        SRTPreemption(&queue, &cpu, time, context_switch, &preem);
        // ===================== I/O BURST COMPLETION =====================
        SRTRemoveProcessFromCPU(&queue, &cpu);
        SRTIOBurstCompletion(processes, &queue, &cpu, time, context_switch, total_processes, &preem);
        // ===================== PROCESS ARRIVAL =====================
        ArrivalTAU(processes, &queue, &cpu, time, total_processes, context_switch);
        ct_switch += InsertProcessToCPU(&queue, &cpu, context_switch);
        // ===================== PROCESS END =====================
        ProcessWaitTime(&queue, &cpu, processes, total_processes);
        ProcessEstimatedWaitTime(&cpu);
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
    printf("time %dms: Simulator ended for SRT ", time);
    print(1, "SRT", processes, wait_time / (ct_switch - preem), ct_switch, preem, util / time, total_processes, wait_time, context_switch);
    printQueue(&queue);
    printf("\n");
    freeQueue(&queue);
}

#endif