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

double next_exp(double lambda, int upper_bound) {
    double d = drand48();
    double res = -log(d) / lambda;
    while (res > upper_bound) res = -log(drand48()) / lambda;
    return res;
}
void exp_random() {
//    srand48( time( NULL ) );
    srand48(1024);

    /* uniform to exponential distribution: */
    /*                                      */
    double min = 0;         /*        -ln(r)                        */
    double max = 0;         /*  x = ----------                      */
    double sum = 0;         /*        lambda                        */
    /*                                      */
    double lambda = 0.001;  /* average should be 1/lambda ===> 1000 */

    int iterations = 10000000;  /* <== make this number very large */

    for ( int i = 0 ; i < iterations ; i++ ) {
        double r = drand48();  /* uniform dist [0.00,1.00) -- also see random() */

        /* generate the next pseudo-random value x */
        double x = -log( r ) / lambda;   /* log() is natural log (see man page) */
#if 0
        /* skip values that are far down the "long tail" of the distribution */
    if ( x > 3000 ) { i--; continue; }

    /* TO DO: Since adding the above line of code will lower the
     *         resulting average, try to modify lamdba to get back
     *          to an average of 1000
     */
#endif
        /* display the first 20 pseudo-random values */
        if ( i < 20 ) printf( "x is %lf\n", x );
        sum += x;
        if ( i == 0 || x < min ) { min = x; }
        if ( i == 0 || x > max ) { max = x; }
    }

    double avg = sum / iterations;

    printf( "minimum value: %lf\n", min );
    printf( "maximum value: %lf\n", max );
    printf( "average value: %lf\n", avg );
}

void sjf(vector<int>& arrival_times, vector<list<int>>& cpu_bursts, vector<list<int>>& io_bursts, int t_cs, float alpha, double lambda);
//void sjf(vector<int> arrival_times, vector<list<int>> cpu_bursts, vector<list<int>> io_bursts, int t_cs, float alpha, double lambda);
void srt(vector<int> arrival_times, vector<list<int>> cpu_bursts, vector<list<int>> io_bursts, int t_cs, float alpha);
void rr(vector<int> arrival_times, vector<list<int>> cpu_bursts, vector<list<int>> io_bursts, int t_cs, float alpha, int t_slice);
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
                    print_queue(queue);
                } else {
                    if (t < 10000) {
                        cout << "time " << t << "ms: Process " << proc << " completed a CPU burst; "
                             << cpu_bursts[proc - 'A'].size() << " burst";
                        if (cpu_bursts[proc - 'A'].size() > 1) cout << "s";
                        if (t < 10000)
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
                if (t < 10000)
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
                    if (t < 10000)
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
                    if (t < 10000)
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
                if (t < 10000)
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

int main(int argc, char** argv) {
    int num_process = atoi(argv[1]);
    int num_cpu_bound = atoi(argv[2]);
    int seed = atoi(argv[3]);
    double lambda = atof(argv[4]);
    int upper_bound = atoi(argv[5]);
    int t_cs = atoi(argv[6]);
    float alpha = atof(argv[7]);
    int t_slice = atoi(argv[8]);

    vector<list<int>> cpu_bursts(num_process);
    vector<list<int>> io_bursts(num_process);
    vector<int> arrival_times(num_process);

    srand48(seed);
    cout << "<<< PROJECT PART I -- process set (n=" << num_process << ") with " << num_cpu_bound << " CPU-bound process";
    if (num_cpu_bound == 1) cout <<  " >>>" << endl;
    else cout << "es >>>" << endl;
    int arrival, num_bursts, cpu_burst, io_burst;
    for (int i = 0; i < num_process; i++) {
        char process = i + 'A';
        // IO bound
        arrival = floor(next_exp(lambda, upper_bound));
        num_bursts = ceil(drand48() * 64);
        if (i < num_process - num_cpu_bound) printf("I/O-bound process %c: arrival time %dms; %d CPU burst", process, arrival, num_bursts);
        else printf("CPU-bound process %c: arrival time %dms; %d CPU burst", process, arrival, num_bursts);
        if (num_bursts > 1) printf("s\n");
        arrival_times[i] = arrival;
        for (int j = 1; j <= num_bursts; j++) {
            // io bound
            if (i < num_process - num_cpu_bound) {
                cpu_burst = ceil(next_exp(lambda, upper_bound));
                cpu_bursts[i].push_back(cpu_burst);
                // last burst
                if (j != num_bursts) {
                    io_burst = ceil(next_exp(lambda, upper_bound)) * 10;
                    io_bursts[i].push_back(io_burst);
                }
            }
            // cpu bound
            else {
                cpu_burst = ceil(next_exp(lambda, upper_bound)) * 4;
                cpu_bursts[i].push_back(cpu_burst);
                // last burst
                if (j != num_bursts) {
                    io_burst = ceil(next_exp(lambda, upper_bound)) * 10 / 8;
                    io_bursts[i].push_back(io_burst);
                }
            }
        }
    }

    // todo: maybe switch to Lists to avoid overhead from switching order of the queue?
    cout << endl << "<<< PROJECT PART II -- t_cs=" << t_cs << "ms; alpha=" << alpha << "; t_slice=" << t_slice << "ms >>>" << endl;
    fcfs(arrival_times, cpu_bursts, io_bursts, t_cs);
    cout << endl;
    sjf(arrival_times, cpu_bursts, io_bursts, t_cs, alpha, lambda);

    return 0;
}

void sjf(vector<int>& arrival_times, vector<list<int>>& cpu_bursts, vector<list<int>>& io_bursts, int t_cs, float alpha, double lambda) {
    /*
     * Shortest job first (SJF)
In SJF, processes are stored in the ready queue in order of priority based on their anticipated CPU
burst times. More specifically, the process with the shortest predicted CPU burst time will be
selected as the next process executed by the CPU.
     */

    int t = 0;
    // process ready queue; all the processes inside have already arrived and are ready to process
    list<char> queue;
    list<int> io_burst_queue(cpu_bursts.size(), -1);

    // executing process
    char proc = 0;
    // whether cpu is running
    bool running = false;
    int switching = 0;
    int last_burst = 0;
    vector<int> tau(cpu_bursts.size(), ceil(1/lambda));

//    tau = ceil(alpha * cpu_burst + (1.0 - alpha) * tau)
    while (true) {
        // start of simulation
        if (t == 0) {
            cout << "time 0ms: Simulator started for SJF [Q <empty>]" << endl;
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
                    print_queue(queue);
                } else {
                    if (t < 10000)
                        cout << "time " << t << "ms: Process " << proc  << " (tau " << tau[proc-'A'] << "ms)" <<
                    " completed a CPU burst; "
                         << cpu_bursts[proc - 'A'].size() << " burst";
                    if (t < 10000)
                        if (cpu_bursts[proc - 'A'].size() > 1) cout << "s";
                    if (t < 10000)
                        cout << " to go";
                    if (t < 10000)
                        print_queue(queue);
                    if (t < 10000)
                        cout << "time " << t << "ms: Recalculating tau for process " << proc <<
                    ": old tau " << tau[proc-'A'];
                    tau[proc-'A'] = ceil(alpha * last_burst + (1.0 - alpha) * tau[proc-'A']);
                    if (t < 10000)
                        cout << "ms ==> new tau " << tau[proc-'A'] << "ms";
                    if (t < 10000)
                        print_queue(queue);


                    auto it = io_burst_queue.begin();
                    advance(it, proc - 'A');
                    *it = t + t_cs / 2 + io_bursts[proc - 'A'].front();
                    if (t < 10000)
                        cout << "time " << t << "ms: Process " << proc
                         << " switching out of CPU; blocking on I/O until time " << *it << "ms";
                }
                running = false;
                if (t < 10000)
                    print_queue(queue);
                switching = t_cs;
            }
        }
        // (c) process starts using the CPU
        if (!running && !switching) {
            if (!queue.empty()) {
                proc = queue.front();
                if (t >= arrival_times[proc - 'A'] + t_cs / 2) {
                    list<int> running_process = cpu_bursts[proc - 'A'];
                    if (t < 10000)
                        cout << "time " << t << "ms: Process " << proc  << " (tau " << tau[proc-'A'] << "ms)"
                    <<" started using the CPU for " << running_process.front() << "ms burst";
                    queue.pop_front();
                    if (t < 10000)
                        print_queue(queue);
                    last_burst = running_process.front();
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
                        cout << "time " << t << "ms: Process " << char(i+'A')  << " (tau " << tau[i] << "ms)"
                    <<" completed I/O; added to ready queue";
                    queue.push_back(char(i+'A'));
                    queue.sort([&tau] (const char& a, const char& b) {
                        return (tau[a-'A'] < tau[b-'A']) || (tau[a-'A'] == tau[b-'A'] && a < b);
                    });
                    if (t < 10000)
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
                    cout << "time " << t << "ms: Process " << char(i+'A') << " (tau " << tau[i] << "ms)" <<
                " arrived; added to ready queue";
                queue.push_back(char(i+'A'));
               queue.sort([&tau] (const char& a, const char& b) {
                    return (tau[a-'A'] < tau[b-'A']) || (tau[a-'A'] == tau[b-'A'] && a < b);
                });
                if (t < 10000)
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
                t += t_cs / 2;
                cout << "time " << t << "ms: Simulator ended for SJF [Q <empty>]" << endl;
                return;
            }
        }
        if (switching) {
            switching--;
        }
        t++;
        if (t > 500000) return;
    }
}
