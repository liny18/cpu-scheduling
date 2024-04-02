# CPU Scheduling Simulator

# TODO
- [ ] All: Possible error in ~~addqueue()~~ enqueueTAU() where it may not be alphabetically sorted
- [ ] All : Maybe switch the language to either C++ or Python?
    - Ignored for now.
- [X] All : Change name of addqueue() in queue.h to something else
    - Switched to enqueueTAU()
------------------------------------------------------------
- [ ] Tyler : Maybe fix the usage when invalid number of args were provided
- [x] Tyler : Start project!!!
------------------------------------------------------------
- [x] Zhi : Added in all the other arguments
- [x] Zhi : Identifying and Defining all of values of distribution for CPU bursts, arrival times, and   CPU burst time with I/O burst time
- [X] Zhi : Implement queue in C
    - added in "bool.h"
    - added in "queue.h"
- [X] Zhi : Memory deallocation for processes and queue
- [X] Zhi : Completed FCFS algorithm
    - updated the queue so that a node has a Process* instead of Process, so we can reference it instead, errors occur if you dont reference a process
    - added in "fcfs.h"
    - added in "cpu.h" as a way to reference what the CPU is doing, a CPU also holds a process when it is processing the process
    - added in "algorithm_helper.h" <--- used for centralizing algorithm functions
- [X] Zhi : Complete RR algorithm
    - added in "rr.h"
        - note that some RR algorithms share mostly the same for FCFS, most functions are reused, and some of those reused are only altered by adding a time slice
- [X] Zhi : Complete SJF algorithm
    - added in "sjf.h"
- [X] Zhi : Complete SRT algorithm
    - added in "srt.h"
- [X] Zhi : Main updated to run with all algorithms
    - forked processes to run (not parallel)
- [X] Zhi : Assured correct algorithms
    - tested our output with sample outputs given
- [X] Zhi : Organize all of the functions and comments

Current Notes:
- ask for any assistance, may write some architecture later on (most of it should be explained through code)
------------------------------------------------------------