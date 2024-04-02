#ifndef __RR__
#define __RR__

#include <limits.h>
#include "process.h"
#include "queue.h"
#include "bool.h"
#include "algorithm_helper.h"
#include "cpu.h"
#include "fcfs.h"
#include "print.h"

// Forward Declaration
void RRAlgorithm(Process* processes, int total_processes, int context_switch, int time_slice);
void RRCPUProcessBegin(Queue* queue, CPU* cpu, int time, int time_slice, int* timer);
void RRPreemption(Queue* queue, CPU* cpu, int time, int* timer, int time_slice, int context_switch, int* preem);

// Begin to process in the CPU
void RRCPUProcessBegin(Queue* queue, CPU* cpu, int time, int time_slice, int* timer){
    if(cpu->state == OCCUPIED && cpu->p->state == CONTEXT_SWITCH_IN && cpu->p->current_wait_time == 0){
        StartPreemptiveBurst(queue, cpu->p, cpu, time);
        *timer = time_slice;
    }
}

/* Tracks pre-emption
    Two different starting states:
    - When a CPU is not occupied
        reset the timer to its time slice
    - When a CPU is occupied with timer = 0 (time slice expired)
        - if the queue is empty
            no preemptions need to happen since there are no processes that preempt
        - queue has processes
            context switch out the current CPU process
            stall the process until its CPU burst begins
    When a CPU is occupied with timer > 0: (make sure this is after the previous decisions)
        tick down the timer
*/
void RRPreemption(Queue* queue, CPU* cpu, int time, int* timer, int time_slice, int context_switch, int* preem){
    if(*timer == 0 && cpu->state == OCCUPIED && cpu->p->state == RUNNING && cpu->p->current_wait_time != 0){
        if(isEmpty(queue)){ // if the queue is empty then no pre-emption is required
            if (ableToPrint(time)) {
                printf("time %dms: Time slice expired; no preemption because ready queue is empty ", time);
                printQueue(queue);
                printf("\n");
            }
            *timer = time_slice;
        } else {
            if (ableToPrint(time)) {
                printf("time %dms: Time slice expired; process %c preempted with %dms remaining ", time, cpu->p->name, cpu->p->current_wait_time);
                printQueue(queue);
                printf("\n");
            }
            *preem += 1;
            // pause current process and enqueue to ready if it still can run
            cpu->p->save_wait_time = cpu->p->current_wait_time;
            cpu->p->current_wait_time = 0;
            ContextSwitch(cpu, cpu->p, context_switch);
            cpu->p->state = CONTEXT_SWITCH_OUT;
            cpu->p->p_state = STALLING;
            *timer = time_slice;
        }
    } else if(cpu->state == NONE){ // reset the timer once the CPU had no processes running
        *timer = time_slice;
    }
    if(*timer > 0 && cpu->state == OCCUPIED){
        *timer -= 1;
    }
}

// Main algorithm for RR
void RRAlgorithm(Process* processes, int total_processes, int context_switch, int time_slice){
    // ===================== DECLARATION/INITIALIZATION =====================
    unsigned int time = 0;
    Queue queue;
    CPU cpu;
    initializeQueue(&queue);
    initializeCPU(&cpu);
    int termination_check = 0;
    double util = 0;
    double wait_time = 0;
    int preem = 0;
    int ct_switch = 0;
    int timer = -1;
    // ===================== BEGIN SIMULATION =====================
    printf("time %dms: Simulator started for RR with time slice %dms ", time, time_slice);
    printQueue(&queue);
    printf("\n");
    while(time < UINT_MAX){
        // ===================== CPU BURST COMPLETION =====================
        FCFSCPUBurstComplete(&queue, &cpu, time, context_switch, &termination_check);
        // ===================== CPU PROCESS BEGIN =====================
        RRCPUProcessBegin(&queue, &cpu, time, time_slice, &timer);
        // ===================== PREEMPTION =====================
        RRPreemption(&queue, &cpu, time, &timer, time_slice, context_switch, &preem);
        // ===================== I/O BURST COMPLETION =====================
        RemoveProcessFromCPU(&queue, &cpu);
        FCFSIOBurstCompletion(processes, &queue, &cpu, time, context_switch, total_processes);
        // ===================== PROCESS ARRIVAL =====================
        Arrival(processes, &queue, &cpu, time, total_processes, context_switch);
        ct_switch += InsertProcessToCPU(&queue, &cpu, context_switch);
        // ===================== PROCESS END =====================
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
    printf("time %dms: Simulator ended for RR ", time);
    print(1, "RR", processes, wait_time / (ct_switch - preem), ct_switch, preem, util / time, total_processes, wait_time, context_switch);
    printQueue(&queue);
    printf("\n");
    freeQueue(&queue);
}

#endif