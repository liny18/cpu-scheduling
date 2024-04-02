//
// Created by meow on 8/12/2023.
//
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <time.h>
#include <list>
#include <list>
#include <vector>
using namespace std;

void print_queue(list<char> queue) {
    if (queue.empty()) cout << " [Q <empty>]" << endl;
    else {
        cout << " [Q";
        for (auto it = queue.begin(); it != queue.end(); it++) {
            cout << " " << *it;
        }
        cout << "]" << endl;
    }
}

void fcfs(vector<int> arrival_times, vector<list<int>> cpu_bursts, vector<list<int>> io_bursts, int t_cs) {
    int t = 0;
    // process ready queue; all the processes inside have already arrived and are ready to process
    list<char> queue;
//    vector<int> start_times(cpu_bursts.size(), -1);
    list<int> io_burst_queue(cpu_bursts.size(), -1);

    // executing process
    char proc = 0;
    // whether cpu is running
    bool running = false;
    int switching = 0;
    while (true) {
        // start of simulation
        if (t == 0) {
            cout << "time 0ms: Simulator started for FCFS [Q <empty>]" << endl;
        }

        /*
         * (d) CPU burst completion > (c) process starts using the CPU >
         * (h) I/O burst completions > (b + h? or just b?) new process arrivals
         */

        // todo: directly modify cpu_bursts to count?? i.e. t++ => cpu_bursts[pid][0]--

        // check cpu burst completion
        if (running) {
            // retrieve list of cpu_burst for running process
            list<int> running_process = cpu_bursts[proc-'A'];
            // if the first process in the queue has its first cpu_burst down to 0, the burst completes
            if (cpu_bursts[proc-'A'].front() == 0) {
                cpu_bursts[proc-'A'].pop_front();
                // no more bursts to go => process terminates
                if (cpu_bursts[proc-'A'].empty()) {
                    cout << "time " << t << "ms: Process " << proc << " terminated";
                } else {
                    if (t < 10000) {
                        cout << "time " << t << "ms: Process " << proc << " completed a CPU burst; "
                             << cpu_bursts[proc - 'A'].size() << " burst";
                        if (cpu_bursts[proc - 'A'].size() > 1) cout << "s";
                        cout << " to go";
                        print_queue(queue);
                    }
                    auto it = io_burst_queue.begin();
                    advance(it, proc - 'A');
                    *it = t + t_cs / 2 + io_bursts[proc - 'A'].front();
                    if (t < 10000)
                        cout << "time " << t << "ms: Process " << proc
                         << " switching out of CPU; blocking on I/O until time " << *it << "ms";
                }
                running = false;
                print_queue(queue);
                t += t_cs;
            }
        }
        // (c) process starts using the CPU
        if (!running && !switching) {
            if (!queue.empty()) {
                proc = queue.front();
                if (t >= arrival_times[proc - 'A'] + t_cs / 2) {
                    list<int> running_process = cpu_bursts[proc - 'A'];
                    if (t < 10000)
                        cout << "time " << t << "ms: Process " << proc << " started using the CPU for " << running_process.front() << "ms burst";
                    queue.pop_front();
                    print_queue(queue);
                    running = true;
                }
            }
        }

        // (h) I/O burst completions
        if (!io_burst_queue.empty()) {
            int i = 0;
            // sorted already in pid
            for (auto it = io_burst_queue.begin(); it != io_burst_queue.end(); it++, i++) {
                if (t == *it) {
                    if (t < 10000)
                        cout << "time " << t << "ms: Process " << char(i+'A') << " completed I/O; added to ready queue";
                    queue.push_back(char(i+'A'));
                    print_queue(queue);
                    io_bursts[i].pop_front();
                    switching = t_cs / 2;
                    break;
                }
            }
        }

        // (b) new process arrivals
        for (long unsigned int i = 0; i < arrival_times.size(); i++) {
            if (t == arrival_times[i]) {
                if (t < 10000)
                    cout << "time " << t << "ms: Process " << char(i+'A') << " arrived; added to ready queue";
                queue.push_back(char(i+'A'));
                print_queue(queue);
            }
        }

        if (running) {
            cpu_bursts[proc - 'A'].front() -= 1;
        } // check if terminated
        else {
            long unsigned int done = 0;
            for (long unsigned int i = 0; i < cpu_bursts.size(); i++) {
                done += cpu_bursts[i].empty();
            }
            if (done == cpu_bursts.size()) {
                // subtract the extra t_cs added
                t -= t_cs / 2;
                cout << "time " << t << "ms: Simulator ended for FCFS [Q <empty>]" << endl;
                return;
            }
        }
        if (switching) {
            switching--;
        }
        t++;
    }
}
