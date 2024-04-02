#ifndef __ALGORITHM_HELPER__
#define __ALGORITHM_HELPER__

#include "process.h"
#include "queue.h"
#include "bool.h"
#include "cpu.h"
#include "print.h"

// ============================================ FORWARD DECLARATION ============================================
// DEFAULT ALGORITHM HELPERS
void Arrival(Process* processes, Queue* queue, CPU* cpu, int time, int total_processes, int context_switch);
int InsertProcessToCPU(Queue* queue, CPU* cpu, int context_switch);
void RemoveProcessFromCPU(Queue* queue, CPU* cpu);
void ProcessWaitTime(Queue* queue, CPU* cpu, Process* processes, int total_processes);
void ProcessEstimatedWaitTime(CPU* cpu);
void ContextSwitch(CPU* cpu, Process* process, int context_switch);
void StartBurst(Queue* queue, Process* process, CPU* cpu, int time);
void StartPreemptiveBurst(Queue* queue, Process* process, CPU* cpu, int time);
void EndBurst(Queue* queue, CPU* cpu, int time);
void BeginIO(Queue* queue, CPU* cpu, int time, int context_switch);
void IOWaitDone(CPU* cpu);
void BeginStalling(Queue* queue, CPU* cpu);
void IOBurst(Process* process, Queue* queue, int time);
void Terminate(Queue* queue, CPU* cpu, int time);

// TAU ALGORITHM HELPERS
void ArrivalTAU(Process* processes, Queue* queue, CPU* cpu, int time, int total_processes, int context_switch);
void RecalculateTAU(Queue* queue, Process* process, int time, double alpha);
void StartBurstTAU(Queue* queue, Process* process, CPU* cpu, int time);
void StartPreemptiveBurstTAU(Queue* queue, Process* process, CPU* cpu, int time);
void EndBurstTAU(Queue* queue, CPU* cpu, int time);
void IOBurstTAU(Process* process, Queue* queue, int time);
void IOBurstPreemptiveTAU(Process* minor_process, Process* major_process, Queue* queue, int time);

// DEBUGGING TOOLS
void DebugCPU(CPU* cpu, int time);
void DebugAllProcesses(Process* processes, int time, int total_processes);

// =============================================================================================================
// ========================================= DEFAULT ALGORITHM HELPERS =========================================
// =============================================================================================================

// Enqueues a process when it arrives at the time
void Arrival(Process* processes, Queue* queue, CPU* cpu, int time, int total_processes, int context_switch){
    for(int x = 0; x < total_processes; x++){
        if(processes[x].arrival_time == time && processes[x].state == ENTRY){
            processes[x].state = READY;
            // estimate the wait time based on context switch
            enqueue(queue, processes+x);
            if (ableToPrint(time)) {
                printf("time %dms: Process %c arrived; added to ready queue ", time, processes[x].name);
                printQueue(queue);
                printf("\n");
            }
        }
    }
}

// Begins context switch to insert the process to the CPU
int InsertProcessToCPU(Queue* queue, CPU* cpu, int context_switch){
    if(cpu->state == OCCUPIED || isEmpty(queue))
        return 0;
    // Context switch
    Process* process = queue->front->process;
    if(process->state == READY && process->current_wait_time == 0){
        dequeue(queue);
        ContextSwitch(cpu, process, context_switch);
        process->state = CONTEXT_SWITCH_IN;
        cpu->state = OCCUPIED;
        cpu->p = process;
        return 1;
    }
    return 0;
}

/*  Remove a process from the CPU once context switch out is finished running
    Based on the processing state:
        - DEFAULT
            the process has finished and will move to I/O
        - STALLING
            the process has been removed for pre-emptive reasons, 
            and will begin stalling in the ready queue
*/
void RemoveProcessFromCPU(Queue* queue, CPU* cpu){
    if(cpu->state == OCCUPIED && cpu->p->state == CONTEXT_SWITCH_OUT && cpu->p->current_wait_time == 0){
        switch(cpu->p->p_state){
            case DEFAULT:
                IOWaitDone(cpu);
                break;
            case STALLING:
                BeginStalling(queue, cpu);
                break;
        }
    }
}

// if there is a wait time for a process, then we decrement it
// THIS PROCESS MUST BE: (OR)
//      - WAITING                               -> decrease IO time
//      - CONTEXT_SWITCH_IN                     -> context switch from READY QUEUE
//      - CONTEXT_SWITCH_OUT                    -> context switch from CPU BEING OCCUPIED
//      - RUNNING                               -> completing burst
//      - TERMINATE                             -> terminate
void ProcessWaitTime(Queue* queue, CPU* cpu, Process* processes, int total_processes){
    for(int x = 0; x < total_processes; x++){
        if(processes[x].state == WAITING         || processes[x].state == CONTEXT_SWITCH_IN ||
        processes[x].state == CONTEXT_SWITCH_OUT || processes[x].state == RUNNING ||
        processes[x].state == TERMINATED){
            if(processes[x].current_wait_time > 0){
                (processes+x)->current_wait_time -= 1;
            }
        }
    }
}

// Process the estimated wait time if required
void ProcessEstimatedWaitTime(CPU* cpu){
    if(cpu->state == OCCUPIED && cpu->p->state != CONTEXT_SWITCH_IN && cpu->p->state != CONTEXT_SWITCH_OUT)
        cpu->p->estimated_wait_time -= 1;
}

// Add estimated wait time based on the context switch
void ContextSwitch(CPU* cpu, Process* process, int context_switch){
    process->current_wait_time = context_switch / 2;
}

// BEGIN A CPU BURST
// NOTE: this will begin running the process
void StartBurst(Queue* queue, Process* process, CPU* cpu, int time){
    process->current_wait_time = process->cpu_burst_times[process->current_burst];
    if (ableToPrint(time)) {
        printf("time %dms: Process %c started using the CPU for %dms burst ", time, process->name, process->current_wait_time);
        printQueue(queue);
        printf("\n");
    }
    process->state = RUNNING;
}

// BEGIN A CPU BURST IF THAT BURST IS PREEMPTIVE
// NOTE: this will begin running the process
void StartPreemptiveBurst(Queue* queue, Process* process, CPU* cpu, int time){
    if(process->save_wait_time > 0){
        process->current_wait_time = process->save_wait_time;
        process->save_wait_time = 0;
        if (ableToPrint(time)) {
            printf("time %dms: Process %c started using the CPU for remaining %dms of %dms burst ", time, process->name, process->current_wait_time, process->cpu_burst_times[process->current_burst]);
        }
    } else {
        process->current_wait_time = process->cpu_burst_times[process->current_burst];
        if (ableToPrint(time)) {
            printf("time %dms: Process %c started using the CPU for %dms burst ", time, process->name, process->current_wait_time);
        }
    }
    if (ableToPrint(time)) {
        printQueue(queue);
        printf("\n");
    }
    process->state = RUNNING;
}

// completed a CPU burst
// NOTE: we didnt unoccupy the CPU because it has to be removed first
void EndBurst(Queue* queue, CPU* cpu, int time){
    Process* temp = cpu->p;
    temp->current_burst += 1;
    int remaining_bursts = temp->total_bursts - temp->current_burst;
    if (ableToPrint(time)) {
        if(remaining_bursts > 1)
            printf("time %dms: Process %c completed a CPU burst; %d bursts to go ", time, temp->name, remaining_bursts);
        else
            printf("time %dms: Process %c completed a CPU burst; %d burst to go ", time, temp->name, remaining_bursts);
        printQueue(queue);
        printf("\n");
    }
    cpu->p->p_state = DEFAULT;
}

// Begin switching out of the CPU to the I/O block
void BeginIO(Queue* queue, CPU* cpu, int time, int context_switch){
    Process* temp = cpu->p;
    int io_burst = temp->io_burst_times[temp->current_io];
    int estimated_time = time + io_burst + context_switch / 2;
    if (ableToPrint(time)) {
        printf("time %dms: Process %c switching out of CPU; will block on I/O until time %dms ", time, temp->name, estimated_time);
        printQueue(queue);
        printf("\n");
    }
    temp->state = CONTEXT_SWITCH_OUT;
    temp->current_wait_time = context_switch / 2;
}

// Finish exiting out of the CPU and now waiting on the I/O block
void IOWaitDone(CPU* cpu){
    int io_burst = cpu->p->io_burst_times[cpu->p->current_io];
    cpu->p->current_io += 1;
    cpu->p->current_wait_time += io_burst;
    cpu->p->state = WAITING;
    cpu->state = NONE;
    cpu->p = NULL;
}

// Stall a process
void BeginStalling(Queue* queue, CPU* cpu){
    cpu->p->current_wait_time = 0;
    cpu->p->state = READY;
    enqueue(queue, cpu->p);
    cpu->state = NONE;
    cpu->p = NULL;
}

// Complete an IO burst, adding it to the ready queue
void IOBurst(Process* process, Queue* queue, int time){
    process->state = READY;
    enqueue(queue, process);
    if (ableToPrint(time)) {
        printf("time %dms: Process %c completed I/O; added to ready queue ", time, process->name);
        printQueue(queue);
        printf("\n");
    }
}

// Terminate a process
void Terminate(Queue* queue, CPU* cpu, int time){
    printf("time %dms: Process %c terminated ", time, cpu->p->name);
    printQueue(queue);
    printf("\n");
    cpu->p->state = TERMINATED;
}

// =============================================================================================================
// =========================================== TAU ALGORITHM HELPERS ===========================================
// =============================================================================================================

// Enqueues a process when it arrives at the time
// NOTE: this prints out the tau value
void ArrivalTAU(Process* processes, Queue* queue, CPU* cpu, int time, int total_processes, int context_switch){
    for(int x = 0; x < total_processes; x++){
        if(processes[x].arrival_time == time && processes[x].state == ENTRY){
            processes[x].state = READY;
            // estimate the wait time based on context switch
            enqueueEstimated(queue, processes+x);
            if (ableToPrint(time)) {
                printf("time %dms: Process %c (tau %dms) arrived; added to ready queue ", time, processes[x].name, processes[x].tau);
                printQueue(queue);
                printf("\n");
            }
        }
    }
}

// Recalculate the next tau value
void RecalculateTAU(Queue* queue, Process* process, int time, double alpha){
    // tau(n+1) = alpha * t + (1-alpha) * tau(n)
    int new_tau = ceil((alpha * process->cpu_burst_times[process->current_burst-1]) 
                    + ((1 - alpha) * process->tau));

    if (ableToPrint(time)) {
        printf("time %dms: Recalculated tau for process %c: old tau %dms; new tau %dms ", time, process->name, process->tau, new_tau);
        printQueue(queue);
        printf("\n");
    }
    process->tau = new_tau;
    process->estimated_wait_time = new_tau;
}

// BEGIN A CPU BURST with tau
// NOTE: this will begin running the process
void StartBurstTAU(Queue* queue, Process* process, CPU* cpu, int time){
    process->current_wait_time = process->cpu_burst_times[process->current_burst];
    if (ableToPrint(time)) {
        printf("time %dms: Process %c (tau %dms) started using the CPU for %dms burst ", time, process->name, process->tau, process->current_wait_time);
        printQueue(queue);
        printf("\n");
    }
    process->state = RUNNING;
}

// BEGIN A CPU BURST IF THAT BURST IS PREEMPTIVE WITH TAU
// NOTE: this will begin running the process
void StartPreemptiveBurstTAU(Queue* queue, Process* process, CPU* cpu, int time){
    if(process->save_wait_time > 0){
        process->current_wait_time = process->save_wait_time;
        process->save_wait_time = 0;
        if (ableToPrint(time)) {
            printf("time %dms: Process %c (tau %dms) started using the CPU for remaining %dms of %dms burst ", time, process->name, process->tau, process->current_wait_time, process->cpu_burst_times[process->current_burst]);
        }
    } else {
        process->current_wait_time = process->cpu_burst_times[process->current_burst];
        if (ableToPrint(time)) {
            printf("time %dms: Process %c (tau %dms) started using the CPU for %dms burst ", time, process->name, process->tau, process->current_wait_time);
        }
    }
    if (ableToPrint(time)) {
        printQueue(queue);
        printf("\n");
    }
    process->state = RUNNING;
}

// completed a CPU burst with tau
// NOTE: we didnt unoccupy the CPU because it has to be removed first
void EndBurstTAU(Queue* queue, CPU* cpu, int time){
    Process* temp = cpu->p;
    temp->current_burst += 1;
    int remaining_bursts = temp->total_bursts - temp->current_burst;
    if (ableToPrint(time)) {
        if(remaining_bursts > 1)
            printf("time %dms: Process %c (tau %dms) completed a CPU burst; %d bursts to go ", time, temp->name, temp->tau, remaining_bursts);
        else
            printf("time %dms: Process %c (tau %dms) completed a CPU burst; %d burst to go ", time, temp->name, temp->tau, remaining_bursts);
        printQueue(queue);
        printf("\n");
    }
    cpu->p->p_state = DEFAULT;
}

// Complete an IO burst, adding it to the ready queue with estimated wait time and prints out tau
void IOBurstTAU(Process* process, Queue* queue, int time){
    process->state = READY;
    enqueueEstimated(queue, process);
    if (ableToPrint(time)) {
        printf("time %dms: Process %c (tau %dms) completed I/O; added to ready queue ", time, process->name, process->tau);
        printQueue(queue);
        printf("\n");
    }
}

// Preempt a major process over a minor process adding it to the ready queue with 
// estimated wait time and prints out tau
void IOBurstPreemptiveTAU(Process* minor_process, Process* major_process, Queue* queue, int time){
    major_process->state = READY;
    enqueueEstimated(queue, major_process);
    if (ableToPrint(time)) {
        printf("time %dms: Process %c (tau %dms) completed I/O; preempting %c ", time, major_process->name, major_process->tau, minor_process->name);
        printQueue(queue);
        printf("\n");
    }
}

// =============================================================================================================
// ================================================ DEBUGGING ==================================================
// =============================================================================================================

void DebugAllProcesses(Process* processes, int time, int total_processes){
    printf(" ================================ TIME %d ================================ \n\n", time);
    for(int x = 0; x < total_processes; x++){
        printf("PROCESS %c : TOTAL_BURSTS: %d, TAU: %d, ", processes[x].name, processes[x].total_bursts, processes[x].tau);
        printf("STATE: ");
        switch(processes[x].state){
            case RUNNING:
                printf("RUNNING");
                break;
            case READY:
                printf("READY");
                break;
            case WAITING:
                printf("WAITING");
                break;
            case ENTRY:
                printf("ENTRY");
                break;
            case CONTEXT_SWITCH_IN:
                printf("CONTEXT_SWITCH_IN");
                break;
            case CONTEXT_SWITCH_OUT:
                printf("CONTEXT_SWITCH_OUT");
                break;
            case TERMINATED:
                printf("TERMINATED");
                break;
            default:
                printf("ERROR YOU SHOULDN\'T SEE THIS!");
                break;
        }
        printf(", WAIT: %d", processes[x].current_wait_time);
        printf(", ESTIMATED WAIT: %d", processes[x].estimated_wait_time);
        printf("\n\n");
    }
}

void DebugCPU(CPU* cpu, int time){
    printf(" ================================ TIME %d ================================ \n", time);
    if(cpu->p == NULL){
        printf("PROCESS NULL : ");
    } else {
        printf("PROCESS %c : ", cpu->p->name);
    }
    printf("STATE: ");
    switch(cpu->state){
        case NONE:
            printf("NONE");
            break;
        case OCCUPIED:
            printf("OCCUPIED");
            break;
        default:
            printf("ERROR YOU SHOULDN\'T SEE THIS!");
            break;
    }
    printf("\n");
}

#endif