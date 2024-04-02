// #include "util.cpp"
// #include "fcfs_cheap.cpp"
// #include "sjf_cheap.cpp"
// #include "srt_cheap.cpp"
// #include "rr_cheap.cpp"

#include <iostream>
#include <algorithm>
#include <cmath>
#include <vector>
#include <climits>
#include <unordered_map>
#include <iomanip>
#include <string>
#include <list>
#include <set>
#include <map>
#include <iomanip>
#include <cassert>
#include <unordered_map>
#include <fstream>
#include <cstdio>

using namespace std;

//this class will be returned with the data for relevant to the simulation
class Simulation { 
    public:
    int wait_time_cpu = 0, wait_count_cpu = 0, wait_time_io = 0, wait_count_io = 0, wait_time_total = 0, wait_count_total = 0;
    double cpu_burst_cpu = 0.0, cpu_burst_io = 0.0, cpu_burst_total = 0.0;
    double turnaround_cpu = 0.0, turnaround_io = 0.0, turnaround_total = 0.0;
    int cs_cpu = 0, cs_io = 0, cs_total = 0;
    int preemptions_cpu = 0, preemptions_io = 0, preemptions_total = 0;
};

//helper function to print the double with 3 decimal places
void printDouble(double d, ostream& os) {
    // d += 0.0005;
    d *= 1000;
    d = ceil(d);
    d /= 1000;
    os << fixed << setprecision(3) << d;
}


int BOUND = INT_MAX;
double lambda;
int CPU_TIME_CPU = 0, CPU_TIME_IO = 0; // the cpu utilization time for cpu and io bound processes

double next_exp() { 
    double r = drand48();
    while (-log(r) / lambda > BOUND) { 
        r = drand48();
    }
    return -log(r) / lambda;
}

class Process { 
    public:
    Process(char c = '/', bool cpu_bound = false) {
        this->indicator = 0;
        process_id = c;
        this->cpu_bound = cpu_bound;
        double rand_arrival_time = floor(next_exp());
        while (rand_arrival_time > BOUND) { 
            rand_arrival_time = floor(next_exp());
        }
        arrival_time = rand_arrival_time;
        double rand_cpu_burst = ceil(100 * drand48());
        while (rand_cpu_burst > BOUND) { 
            rand_cpu_burst = ceil(100 * next_exp());
        }
        cpu_bursts = rand_cpu_burst;
        times = vector<pair<int, int> >(cpu_bursts);
        for (int i = 0; i < cpu_bursts; i++) { 
            double temp1 = ceil(next_exp());
            while (temp1 > BOUND) { 
                temp1 = ceil(next_exp());
            }
            if (i == cpu_bursts - 1) { 
                times[i] = make_pair(temp1, 0);
            }
            else { 
                double temp2 = ceil(next_exp());
                while (temp2 > BOUND) { 
                    temp2 = ceil(next_exp());
                }
                times[i] = make_pair(temp1, 10 * temp2); //times are stored as {cpu burst, io burst}
            }

            if (cpu_bound) { 
                times[i].first *= 4;
                times[i].second /= 4;                
            }
            if (cpu_bound)
                CPU_TIME_CPU += times[i].first;
            else 
                CPU_TIME_IO += times[i].first;
        }
        //delete the last IO burst 
        times.back().second = 0;
    }
    void printProcess() { 
        if (cpu_bound) { 
            cout << "CPU";
        }
        else { 
            cout << "I/O";
        }
        cout << "-bound process " << process_id << ": arrival time " << arrival_time << "ms; " << cpu_bursts << " CPU bursts:" << endl;
        for (auto p : times) { 
            if (p != times.back())
                cout << "--> CPU burst " << p.first << "ms --> I/O burst " << p.second << "ms" << endl;
            else 
                cout << "--> CPU burst " << p.first << "ms" << endl;
        }
    }
    //this print function is for part II. It prints only the first line.
    void printProcessSummarized() { 
        if (cpu_bound) { 
            cout << "CPU";
        }
        else { 
            cout << "I/O";
        }
        cout << "-bound process " << process_id << ": arrival time " << arrival_time << "ms; " << cpu_bursts << " CPU bursts" << endl;
    }
    void resetProcess() { 
        indicator = 0;
        time_queue = 0;
        wait_time = 0;
        wait_count = 0;
        tau = 1.0 / lambda;
        leftover_tau = tau;
        remaining_time = 0;
        joining_time = 0;
    }

  char process_id;
  int arrival_time, cpu_bursts;
  vector<pair<int, int> > times; 
  bool cpu_bound;
  int indicator = 0; // 0: arrival, 1: cpu start, 2: cpu end, 3: tau recalc, 4: preemption, 5: I/O start, 6: I/O end, 7: process finish 
  int time_queue = 0, wait_time = 0, wait_count = 0; // stores the time that this process was in the ready_queue
  int tau = 1.0 / lambda;
  int leftover_tau = tau; // this is for the SRT algorithm
  int remaining_time = 0; // stores the remaining time for the current burst
  int joining_time = 0; // stores the time when the process joins the ready queue
};

bool operator<(const Process & p1, const Process & p2) { 
    // return true;
    if (p1.indicator == p2.indicator) { 
        return p1.process_id < p2.process_id;
    }
    if (p1.indicator == -1 && p2.indicator != -1) { 
        return true;
    } 
    if (p1.indicator != -1 && p2.indicator == -1) { 
        return false;
    }
    if (p1.indicator == 2 && p2.indicator != 2) { 
        return true;
    }
    if (p1.indicator != 2 && p2.indicator == 2) { 
        return false;
    }
    if (p1.indicator == 1 && p2.indicator != 1) { 
        return true;
    }
    if (p1.indicator != 1 && p2.indicator == 1) { 
        return false;
    }
    if (p1.indicator == 6 && p2.indicator != 6) { 
        return true;
    }
    if (p1.indicator != 6 && p2.indicator == 6) { 
        return false;
    }
    if (p1.indicator == 0 && p2.indicator != 0) { 
        return true;
    }
    return p1.process_id < p2.process_id;
}

bool operator==(const Process & p1, const Process & p2) { 
    return p1.process_id == p2.process_id;
}

vector<Process> global_processes; // this is only for preventing compiler warnings.


//helper function to print the queue
void printQueue(const list<int> & p, const vector<Process> & processes) { 
    cout << "[Q";
    if (p.empty()) { 
        cout << " <empty>]" << endl;
        return;
    }
    for (int i : p) { 
        cout << " " << processes[i].process_id;
    }
    cout << "]\n";
}

//helper function to print the queue
void printQueue(const list<Process> & p) { 
    cout << "[Q";
    if (p.empty()) { 
        cout << " <empty>]" << endl;
        return;
    }
    for (const Process & i : p) { 
        cout << " " << i.process_id;
    }
    cout << "]\n";
}


class Integer { //stores the index of the process. This is needed to compare 
    public:
    Integer(int index, const vector<Process> & processes) { 
        this->index = index; 
        this->processes = processes;
    }
    vector<Process> & processes = global_processes; 
    int index;
};

bool operator<(const Integer & i1, const Integer & i2) { 
    return i1.processes[i1.index] < i1.processes[i2.index];
}

bool operator==(const Integer i1, const Integer i2) { 
    return i1.index == i2.index;
}

void push_to_queue(const vector<Process> & processes, list<int> & ready_queue, int process_index) { //pushes the process to the queue at the space in accordance to the tau value
  if (ready_queue.empty()) { 
      ready_queue.push_back(process_index);
      return;
  }
  list<int>::iterator it = ready_queue.begin();
  while (it != ready_queue.end()) { 
      if (processes[process_index].leftover_tau < processes[*it].leftover_tau || (processes[process_index].leftover_tau == processes[*it].leftover_tau && processes[process_index].process_id < processes[*it].process_id)) { 
        ready_queue.insert(it, process_index);
        return;
      }
      it++;
  }
  ready_queue.push_back(process_index);
}



Simulation simulateFCFS(vector<Process> & processes, int cst) { //cst is the context switch time
  cout << "time 0ms: Simulator started for FCFS [Q <empty>]\n";
  size_t terminated_count = 0;
  list<int> queue; 
  unordered_map<int, set<Integer> > map;
  Simulation s; 
  bool cpu_available = true;
  int time = 1; 
  vector<int> indices(processes.size(), 0);
  for (size_t i = 0; i < processes.size(); i++) { 
      map[processes[i].arrival_time].insert(Integer(i, processes));
  }
  for (;;time++) { 
    // -1: make cpu_available true, 0: arrival, 1: cpu start, 2: cpu end, 3: tau recalc, 4: preemption, 5: I/O start, 6: I/O end, 7: process finish, 8: cpu becomes available, 9: waiting in the queue
    if (terminated_count == processes.size() || map.empty()) break;

    auto it = map.find(time);

    if (it != map.end()) { 
      set<Integer> sorting_set;
      for (auto it2 : it->second) { 
        sorting_set.insert(it2);
      }
      it->second = sorting_set;
    }
    
    while (it != map.end() && !it->second.empty()) {
      Integer temp_int = *map[time].begin(); 
      Process & p = processes[temp_int.index];
      // cout << temp_int.index << endl;
      map[time].erase(map[time].begin());
      if (p.indicator == 0) { 
        if (time < 10000)
            cout << "time " << p.arrival_time << "ms: Process " << p.process_id << " arrived; added to ready queue ";
        queue.push_back(p.process_id - 'A');
        p.time_queue = time;
        if (time < 10000)
            printQueue(queue, processes);
        p.indicator = 1;
      }
      else if (p.indicator == 1) { 
        int process_time = p.times[indices[p.process_id - 'A']].first;
        if (time < 10000)
            cout << "time " << time << "ms: Process " << p.process_id << " started using the CPU for " << process_time << "ms burst ";
        if (time < 10000)
            printQueue(queue, processes);
        map[time + process_time].insert(Integer(temp_int.index, processes));
        p.indicator = 2;
        cpu_available = false;
      }
      else if (p.indicator == 2) { 
        int bursts_left = p.times.size() - indices[p.process_id - 'A'] - 1;
        if (bursts_left == 0) { 
          cout << "time " << time << "ms: Process " << p.process_id << " terminated ";
          printQueue(queue, processes);
          p.indicator = 7;
          terminated_count++;
        }
        else { 
          int io_time = p.times[indices[p.process_id - 'A']].second;
          if (bursts_left > 1) { 
            if (time < 10000)
                cout << "time " << time << "ms: Process " << p.process_id << " completed a CPU burst; " << bursts_left << " bursts to go ";
          }
          else {
            if (time < 10000)
                cout << "time " << time << "ms: Process " << p.process_id << " completed a CPU burst; 1 burst to go ";
          }
          if (time < 10000)
            printQueue(queue, processes);
          if (time < 10000)
          cout << "time " << time << "ms: Process " << p.process_id << " switching out of CPU; blocking on I/O until time " << time + io_time + cst / 2 << "ms ";
          // p.indicator = 6;
          if (time < 10000)
            printQueue(queue, processes);
          map[time + io_time + cst / 2].insert(Integer(temp_int.index, processes));
        }
        p.indicator = -1;
        map[time + cst / 2].insert(Integer(temp_int.index, processes));
        // cpu_available = true;
      }
      else if (p.indicator == -1) { 
        cpu_available = true;
        p.indicator = 6;
      }
      else if (p.indicator == 6) { 
        if (time < 10000)
            cout << "time " << time << "ms: Process " << p.process_id << " completed I/O; added to ready queue ";
        queue.push_back(p.process_id - 'A');
        p.time_queue = time;
        if (time < 10000)
            printQueue(queue, processes);
        p.indicator = 1;
        indices[p.process_id - 'A']++;
      }

    }


    if (cpu_available && !queue.empty()) { 
      Process & p = processes[*queue.begin()];
      queue.pop_front();
      // cout << p.process_id << endl;
      map[time + cst / 2].insert(Integer(p.process_id - 'A', processes));
      p.indicator = 1;
      p.wait_time += time - p.time_queue;
      // cout << "something" << endl;
      cpu_available = false;
    }
  }
  cout << "time " << time - 1 + cst / 2 << "ms: Simulator ended for FCFS [Q <empty>]\n" << endl;

  for (Process & p : processes) { 
      if (p.cpu_bound) { 
          s.wait_time_cpu += p.wait_time;
          s.wait_count_cpu += p.times.size();
      }
      else { 
          s.wait_time_io += p.wait_time;
          s.wait_count_io += p.times.size();
      }
      s.wait_time_total += p.wait_time;
      s.wait_count_total += p.times.size();
  }

  s.cpu_burst_cpu = ((double) CPU_TIME_CPU) / s.wait_count_cpu;
  s.cpu_burst_io = ((double) CPU_TIME_IO) / s.wait_count_io;
  s.cpu_burst_total = (double) ((double) CPU_TIME_IO + (double) CPU_TIME_CPU) / (s.wait_count_io + s.wait_count_cpu);

  s.turnaround_cpu = s.cpu_burst_cpu + (double) ((double) s.wait_time_cpu / (double) s.wait_count_cpu) + cst;
  s.turnaround_io = s.cpu_burst_io + (double) ((double) s.wait_time_io / (double) s.wait_count_io) + cst;
  s.turnaround_total = s.cpu_burst_total + (double) ((double) s.wait_time_total / (double) s.wait_count_total) + cst;

  s.cs_cpu = s.wait_count_cpu;
  s.cs_io = s.wait_count_io;
  s.cs_total = s.wait_count_io + s.wait_count_cpu;

    // remove("simout.txt");
    ofstream outfile("simout.txt", ios::out);
    // Write the output to the file
    outfile << fixed << setprecision(3);
    outfile << "Algorithm FCFS\n";
    outfile << "-- CPU utilization: ";
    printDouble((double) (((double) CPU_TIME_CPU + CPU_TIME_IO) / (double) time) * 100, outfile);
    outfile << "%\n";
    outfile << "-- average CPU burst time: ";
    printDouble(s.cpu_burst_total, outfile);
    outfile << " ms (";
    printDouble(s.cpu_burst_cpu, outfile);
    outfile << " ms/";
    printDouble(s.cpu_burst_io, outfile);
    outfile << " ms)\n";
    outfile << "-- average wait time: ";
    printDouble((double) ((double) s.wait_time_total / (double) s.wait_count_total), outfile);
    outfile << " ms (";
    printDouble((double) ((double) s.wait_time_cpu / (double) s.wait_count_cpu), outfile);
    outfile << " ms/";
    printDouble((double) s.wait_time_io / (double) s.wait_count_io, outfile);
    outfile << " ms)\n";
    outfile << "-- average turnaround time: ";
    printDouble(s.turnaround_total, outfile);
    outfile << " ms (";
    printDouble(s.turnaround_cpu, outfile);
    outfile << " ms/";
    printDouble(s.turnaround_io, outfile);
    outfile << " ms)\n";
    outfile << "-- number of context switches: " << s.cs_total << " (" << s.cs_cpu << "/" << s.cs_io << ")\n";
    outfile << "-- number of preemptions: " << s.preemptions_total << " (" << s.preemptions_cpu << "/" << s.preemptions_io << ")\n";
    outfile << "\n";
    // Close the output file
    outfile.close();

  return s;
}
Simulation simulateSJF(vector<Process> & processes, int cst, float alpha) { //cst is the context switch time

  cout << "time 0ms: Simulator started for SJF [Q <empty>]\n";
  size_t terminated_count = 0;
  list<int> queue; 
  unordered_map<int, set<Integer> > map;
  Simulation s; 
  bool cpu_available = true;
  int time = 1; 
  vector<int> indices(processes.size(), 0);
  for (size_t i = 0; i < processes.size(); i++) { 
      map[processes[i].arrival_time].insert(Integer(i, processes));
      processes[i].resetProcess();
  }
  for (;;time++) { 
    // -1: make cpu_available true, 0: arrival, 1: cpu start, 2: cpu end, 3: tau recalc, 4: preemption, 5: I/O start, 6: I/O end, 7: process finish, 8: cpu becomes available, 9: waiting in the queue
    if (terminated_count == processes.size() || map.empty()) break;

    auto it = map.find(time);

    if (it != map.end()) { 
      set<Integer> sorting_set;
      for (auto it2 : it->second) { 
        sorting_set.insert(it2);
      }
      it->second = sorting_set;
    }
    
    while (it != map.end() && !it->second.empty()) {
      Integer temp_int = *map[time].begin(); 
      Process & p = processes[temp_int.index];
      // cout << temp_int.index << endl;
      map[time].erase(map[time].begin());
      if (p.indicator == 0) { 
        if (time < 10000)
            cout << "time " << p.arrival_time << "ms: Process " << p.process_id << " (tau " << p.tau << "ms) arrived; added to ready queue ";
        push_to_queue(processes, queue, p.process_id - 'A');
        p.time_queue = time;
        if (time < 10000)
            printQueue(queue, processes);
        p.indicator = 1;
      }
      else if (p.indicator == 1) { 
        int process_time = p.times[indices[p.process_id - 'A']].first;
        if (time < 10000)
            cout << "time " << time << "ms: Process " << p.process_id << " (tau " << p.tau << "ms) started using the CPU for " << process_time << "ms burst ";
        if (time < 10000)
            printQueue(queue, processes);
        map[time + process_time].insert(Integer(temp_int.index, processes));
        p.indicator = 2;
        cpu_available = false;
      }
      else if (p.indicator == 2) { 
        int bursts_left = p.times.size() - indices[p.process_id - 'A'] - 1;
        if (bursts_left == 0) { 
          cout << "time " << time << "ms: Process " << p.process_id << " terminated ";
          printQueue(queue, processes);
          p.indicator = 7;
          terminated_count++;
        }
        else { 
          int io_time = p.times[indices[p.process_id - 'A']].second;
          if (bursts_left > 1) { 
            if (time < 10000)
                cout << "time " << time << "ms: Process " << p.process_id << " (tau " << p.tau << "ms) completed a CPU burst; " << bursts_left << " bursts to go ";
          }
          else {
            if (time < 10000)
                cout << "time " << time << "ms: Process " << p.process_id << " (tau " << p.tau << "ms) completed a CPU burst; 1 burst to go ";
          }
          if (time < 10000)
            printQueue(queue, processes);
          int old_tau = p.tau;
          p.tau = ceil((1 - alpha) * p.tau + alpha * ( processes[p.process_id - 'A'].times[indices[p.process_id - 'A']].first));
          if (time < 10000)
            cout << "time " << time << "ms: Recalculating tau for process " << p.process_id << ": old tau " << old_tau << "ms ==> new tau " << p.tau << "ms ";
          if (time < 10000)
            printQueue(queue, processes);
          p.leftover_tau = p.tau;
          if (time < 10000)
            cout << "time " << time << "ms: Process " << p.process_id << " switching out of CPU; blocking on I/O until time " << time + io_time + cst / 2 << "ms ";
          // p.indicator = 6;
          if (time < 10000)
            printQueue(queue, processes);
          map[time + io_time + cst / 2].insert(Integer(temp_int.index, processes));
        }
        p.indicator = -1;
        map[time + cst / 2].insert(Integer(temp_int.index, processes));
        // cpu_available = true;
      }
      else if (p.indicator == -1) { 
        cpu_available = true;
        p.indicator = 6;
      }
      else if (p.indicator == 6) { 
        if (time < 10000)
            cout << "time " << time << "ms: Process " << p.process_id << " (tau " << p.tau << "ms) completed I/O; added to ready queue ";
        push_to_queue(processes, queue, p.process_id - 'A');
        p.time_queue = time;
        if (time < 10000)
            printQueue(queue, processes);
        p.indicator = 1;
        indices[p.process_id - 'A']++;
      }

    }


    if (cpu_available && !queue.empty()) { 
      Process & p = processes[*queue.begin()];
      queue.pop_front();
      // cout << p.process_id << endl;
      map[time + cst / 2].insert(Integer(p.process_id - 'A', processes));
      p.indicator = 1;
      p.wait_time += time - p.time_queue;
      // cout << "something" << endl;
      cpu_available = false;
    }
  }
  cout << "time " << time - 1 + cst / 2 << "ms: Simulator ended for SJF [Q <empty>]\n" << endl;

  for (Process & p : processes) { 
      if (p.cpu_bound) { 
          s.wait_time_cpu += p.wait_time;
          s.wait_count_cpu += p.times.size();
      }
      else { 
          s.wait_time_io += p.wait_time;
          s.wait_count_io += p.times.size();
      }
      s.wait_time_total += p.wait_time;
      s.wait_count_total += p.times.size();
  }

  s.cpu_burst_cpu = ((double) CPU_TIME_CPU) / s.wait_count_cpu;
  s.cpu_burst_io = ((double) CPU_TIME_IO) / s.wait_count_io;
  s.cpu_burst_total = (double) ((double) CPU_TIME_IO + (double) CPU_TIME_CPU) / (s.wait_count_io + s.wait_count_cpu);

  s.turnaround_cpu = s.cpu_burst_cpu + (double) ((double) s.wait_time_cpu / (double) s.wait_count_cpu) + cst;
  s.turnaround_io = s.cpu_burst_io + (double) ((double) s.wait_time_io / (double) s.wait_count_io) + cst;
  s.turnaround_total = s.cpu_burst_total + (double) ((double) s.wait_time_total / (double) s.wait_count_total) + cst;

  s.cs_cpu = s.wait_count_cpu;
  s.cs_io = s.wait_count_io;
  s.cs_total = s.wait_count_io + s.wait_count_cpu;

  ofstream outfile("simout.txt", std::ios_base::app);
  // Write the output to the file
  outfile << fixed << setprecision(3);
  outfile << "Algorithm SJF\n";
  outfile << "-- CPU utilization: ";
  printDouble((double) (((double) CPU_TIME_CPU + CPU_TIME_IO) / (double) time) * 100, outfile);
  outfile << "%\n";
  outfile << "-- average CPU burst time: ";
  printDouble(s.cpu_burst_total, outfile);
  outfile << " ms (";
  printDouble(s.cpu_burst_cpu, outfile);
  outfile << " ms/";
  printDouble(s.cpu_burst_io, outfile);
  outfile << " ms)\n";
  outfile << "-- average wait time: ";
  printDouble((double) ((double) s.wait_time_total / (double) s.wait_count_total), outfile);
  outfile << " ms (";
  printDouble((double) ((double) s.wait_time_cpu / (double) s.wait_count_cpu), outfile);
  outfile << " ms/";
  printDouble((double) s.wait_time_io / (double) s.wait_count_io, outfile);
  outfile << " ms)\n";
  outfile << "-- average turnaround time: ";
  printDouble(s.turnaround_total, outfile);
  outfile << " ms (";
  printDouble(s.turnaround_cpu, outfile);
  outfile << " ms/";
  printDouble(s.turnaround_io, outfile);
  outfile << " ms)\n";
  outfile << "-- number of context switches: " << s.cs_total << " (" << s.cs_cpu << "/" << s.cs_io << ")\n";
  outfile << "-- number of preemptions: " << s.preemptions_total << " (" << s.preemptions_cpu << "/" << s.preemptions_io << ")\n";
  outfile << "\n";
  // Close the output file
  outfile.close();

  return s;
}
Simulation simulateSRT(vector<Process> & processes, int cst, float alpha) { //cst is the context switch time
  cout << "time 0ms: Simulator started for SRT [Q <empty>]\n";
  size_t terminated_count = 0;
  list<int> queue; 
  unordered_map<int, set<Integer> > map;
  Simulation s; 
  bool cpu_available = true;
  int cpu_start_time = 0;
  int time = 1; 
  int cpu_user = -1;
  vector<int> indices(processes.size(), 0);
  for (size_t i = 0; i < processes.size(); i++) { 
      map[processes[i].arrival_time].insert(Integer(i, processes));
      processes[i].resetProcess();
  }
  for (;;time++) { 
    // -1: make cpu_available true, 0: arrival, 1: cpu start, 2: cpu end, 3: tau recalc, 4: preemption, 5: I/O start, 6: I/O end, 7: process finish, 8: cpu becomes available, 9: waiting in the queue
    if (terminated_count == processes.size() || map.empty()) break;
    if (map.empty() && queue.empty()) break;

    auto it = map.find(time);

    if (it != map.end()) { 
      set<Integer> sorting_set;
      for (auto it2 : it->second) { 
        sorting_set.insert(it2);
      }
      it->second = sorting_set;
    }
    
    while (it != map.end() && !it->second.empty()) {
      Integer temp_int = *map[time].begin(); 
      Process & p = processes[temp_int.index];
      // cout << temp_int.index << endl;
      map[time].erase(map[time].begin());
      if (p.indicator == 0) { //process arrival
        //check preemption here: 
        if (!cpu_available && (time - processes[cpu_user].joining_time - cst / 2) >= 0 && processes[cpu_user].leftover_tau - (time - cpu_start_time) > p.tau) { 
          if (time < 10000)
            cout << "time " << p.arrival_time << "ms: Process " << p.process_id << " (tau " << p.tau << "ms) arrived; preempting " << processes[cpu_user].process_id << " ";
          push_to_queue(processes, queue, p.process_id - 'A');
          p.time_queue = time;
          if (time < 10000)
            printQueue(queue, processes);
          processes[cpu_user].leftover_tau -= (time - cpu_start_time);
          map[time + cst / 2].insert(Integer(cpu_user, processes));
          processes[cpu_user].indicator = 4;
          processes[cpu_user].time_queue = time;
          processes[cpu_user].remaining_time -= (time - cpu_start_time);
          //remove the process's finishing time from the map
          map[time + processes[cpu_user].remaining_time].erase(Integer(cpu_user, processes));
          if (map[time + processes[cpu_user].remaining_time].empty()) 
            map.erase(time + processes[cpu_user].remaining_time);
          if (processes[cpu_user].cpu_bound) { 
            s.preemptions_cpu++;
          }
          else { 
            s.preemptions_io++;
          }
          s.preemptions_total++;
        }
        else { 
          if (time < 10000)
            cout << "time " << p.arrival_time << "ms: Process " << p.process_id << " (tau " << p.tau << "ms) arrived; added to ready queue ";
          push_to_queue(processes, queue, p.process_id - 'A');
        }
        p.time_queue = time;
        if (time < 10000)
            printQueue(queue, processes);
        p.indicator = 1;
      }
      else if (p.indicator == 1) { //cpu start
        int process_time = p.times[indices[p.process_id - 'A']].first;
        int process_end_time = time;
        if (p.remaining_time > 0 && p.remaining_time < process_time) { 
          if (time < 10000)
            cout << "time " << time << "ms: Process " << p.process_id << " (tau " << p.tau << "ms) started using the CPU for remaining " << p.remaining_time << "ms of " << process_time << "ms burst ";
          map[time + p.remaining_time].insert(Integer(temp_int.index, processes));
          process_end_time += p.remaining_time;
        }
        else {
          if (time < 10000)
            cout << "time " << time << "ms: Process " << p.process_id << " (tau " << p.tau << "ms) started using the CPU for " << process_time << "ms burst ";
          map[time + process_time].insert(Integer(temp_int.index, processes));
          p.remaining_time = process_time;
          process_end_time += process_time;
        }
        if (time < 10000)
            printQueue(queue, processes);
        if (!queue.empty()) { 
          Process & front = processes[*queue.begin()];
          if (p.leftover_tau > front.leftover_tau) { 
            if (time < 10000)
                cout << "time " << time << "ms: Process " << front.process_id << " (tau " << front.tau << "ms) will preempt " << p.process_id << " ";
            if (time < 10000)
                printQueue(queue, processes);
            // push_to_queue(processes, queue, p.process_id - 'A');
            map[time + cst / 2].insert(Integer(cpu_user, processes));
            // p.time_queue = time;
            p.indicator = 4;
            //remove the process's finishing time from the map
            map[process_end_time].erase(Integer(p.process_id - 'A', processes));
            if (map[process_end_time].empty()) 
              map.erase(process_end_time);
            if (processes[cpu_user].cpu_bound) { 
            s.preemptions_cpu++;
            }
            else { 
              s.preemptions_io++;
            }
            s.preemptions_total++;
            continue;
          }

        }

        p.indicator = 2;
        cpu_available = false;
        cpu_start_time = time;
      }
      else if (p.indicator == 2) { //cpu end
        int bursts_left = p.times.size() - indices[p.process_id - 'A'] - 1;
        if (bursts_left == 0) { 
          cout << "time " << time << "ms: Process " << p.process_id << " terminated ";
          printQueue(queue, processes);
          p.indicator = 7;
          terminated_count++;
        }
        else { 
          int io_time = p.times[indices[p.process_id - 'A']].second;
          if (bursts_left > 1) {
            if (time < 10000)
                cout << "time " << time << "ms: Process " << p.process_id << " (tau " << p.tau << "ms) completed a CPU burst; " << bursts_left << " bursts to go ";
          }
          else {
            if (time < 10000)
                cout << "time " << time << "ms: Process " << p.process_id << " (tau " << p.tau << "ms) completed a CPU burst; 1 burst to go ";
          }
          if (time < 10000)
            printQueue(queue, processes);
          int old_tau = p.tau;
          p.tau = ceil((1 - alpha) * p.tau + alpha * ((float) processes[p.process_id - 'A'].times[indices[p.process_id - 'A']].first));
          if (time < 10000)
            cout << "time " << time << "ms: Recalculating tau for process " << p.process_id << ": old tau " << old_tau << "ms ==> new tau " << p.tau << "ms ";
          if (time < 10000)
            printQueue(queue, processes);
          p.leftover_tau = p.tau;
          if (time < 10000)
            cout << "time " << time << "ms: Process " << p.process_id << " switching out of CPU; blocking on I/O until time " << time + io_time + cst / 2 << "ms ";
          // p.indicator = 6;
          if (time < 10000)
            printQueue(queue, processes);
          map[time + io_time + cst / 2].insert(Integer(temp_int.index, processes));
        }
        p.indicator = -1;
        map[time + cst / 2].insert(Integer(temp_int.index, processes));
        p.remaining_time = 0;
        // cpu_available = true;
      }
      else if (p.indicator == -1) { //this indicates that the process is leaving the cpu
        cpu_available = true;
        p.indicator = 6;
      }
      else if (p.indicator == 4) { //this means that the process is preempted
        cpu_available = true;
        push_to_queue(processes, queue, p.process_id - 'A');
        p.time_queue = time;
      }
      else if (p.indicator == 6) { 
        if (!cpu_available && (time - processes[cpu_user].joining_time - cst / 2) >= 0 && processes[cpu_user].leftover_tau - (time - cpu_start_time) > p.tau) { 
          if (time < 10000)
            cout << "time " << time << "ms: Process " << p.process_id << " (tau " << p.tau << "ms) completed I/O; preempting " << processes[cpu_user].process_id << " ";
          processes[cpu_user].leftover_tau -= (time - cpu_start_time);
          processes[cpu_user].indicator = 4;
          processes[cpu_user].time_queue = time;
          processes[cpu_user].remaining_time -= (time - cpu_start_time);
          //remove the process's finishing time from the map
          map[time + processes[cpu_user].remaining_time].erase(Integer(cpu_user, processes));
          if (map[time + processes[cpu_user].remaining_time].empty()) 
            map.erase(time + processes[cpu_user].remaining_time);
          map[time + cst / 2].insert(Integer(cpu_user, processes));
          push_to_queue(processes, queue, p.process_id - 'A');
          if (time < 10000)
            printQueue(queue, processes);
          if (processes[cpu_user].cpu_bound) { 
            s.preemptions_cpu++;
          }
          else { 
            s.preemptions_io++;
          }
          s.preemptions_total++;
        }
        else { 
          if (time < 10000)
            cout << "time " << time << "ms: Process " << p.process_id << " (tau " << p.tau << "ms) completed I/O; added to ready queue ";
          push_to_queue(processes, queue, p.process_id - 'A');
          p.time_queue = time;
          if (time < 10000)
            printQueue(queue, processes);
        }
        p.indicator = 1;
        indices[p.process_id - 'A']++;
      }

    }


    if (cpu_available && !queue.empty()) { 
      Process & p = processes[*queue.begin()];
      queue.pop_front();
      // cout << p.process_id << endl;
      map[time + cst / 2].insert(Integer(p.process_id - 'A', processes));
      p.indicator = 1;
      p.wait_time += time - p.time_queue;
      cpu_user = p.process_id - 'A';
      // cout << "something" << endl;
      cpu_available = false;
      p.joining_time = time;
    }
  }
  cout << "time " << time - 1 + cst / 2 << "ms: Simulator ended for SRT [Q <empty>]\n" << endl;

  s.wait_count_io = 0;
  s.wait_count_cpu = 0;
  s.wait_count_total = 0;
  for (Process & p : processes) { 
      if (p.cpu_bound) { 
          s.wait_time_cpu += p.wait_time;
          s.wait_count_cpu += p.cpu_bursts;
      }
      else { 
          s.wait_time_io += p.wait_time;
          s.wait_count_io += p.cpu_bursts;
      }
      s.wait_count_total += p.cpu_bursts;
      s.wait_time_total += p.wait_time;
  }
  s.cpu_burst_cpu = ((double) CPU_TIME_CPU) / s.wait_count_cpu;
  s.cpu_burst_io = ((double) CPU_TIME_IO) / s.wait_count_io;
  s.cpu_burst_total = (double) ((double) CPU_TIME_IO + (double) CPU_TIME_CPU) / (s.wait_count_io + s.wait_count_cpu);

  s.turnaround_cpu = s.cpu_burst_cpu + (double) ((double) (s.wait_time_cpu + s.preemptions_cpu * cst) / (double) s.wait_count_cpu) + cst;
  s.turnaround_io = s.cpu_burst_io + (double) ((double) (s.wait_time_io + s.preemptions_io * cst) / (double) s.wait_count_io) + cst;
  s.turnaround_total = s.cpu_burst_total + (double) ((double) (s.wait_time_total + s.preemptions_total * cst) / (double) s.wait_count_total) + cst;

  ofstream outfile("simout.txt", std::ios_base::app);
    // Write the output to the file
    outfile << fixed << setprecision(3);
  outfile << "Algorithm SRT\n";
  outfile << "-- CPU utilization: ";
  printDouble((double) (((double) CPU_TIME_CPU + CPU_TIME_IO) / (double) time) * 100, outfile);
  outfile << "%\n";
  outfile << "-- average CPU burst time: ";
  printDouble(s.cpu_burst_total, outfile);
  outfile << " ms (";
  printDouble(s.cpu_burst_cpu, outfile);
  outfile << " ms/";
  printDouble(s.cpu_burst_io, outfile);
  outfile << " ms)\n";
  outfile << "-- average wait time: ";
  printDouble((double) ((double) s.wait_time_total / (double) s.wait_count_total), outfile);
  outfile << " ms (";
  printDouble((double) ((double) s.wait_time_cpu / (double) s.wait_count_cpu), outfile);
  outfile<< " ms/";
  printDouble((double) s.wait_time_io / (double) s.wait_count_io, outfile);
  outfile << " ms)\n";
  outfile << "-- average turnaround time: ";
  printDouble(s.turnaround_total, outfile);
  outfile << " ms (";
  printDouble(s.turnaround_cpu, outfile);
  outfile << " ms/";
  printDouble(s.turnaround_io, outfile);
  outfile << " ms)\n";

  s.wait_count_cpu += s.preemptions_cpu;
  s.wait_count_io += s.preemptions_io;
  s.wait_count_total += s.preemptions_total;

  outfile << "-- number of context switches: " << s.wait_count_total << " (" << s.wait_count_cpu << "/" << s.wait_count_io << ")\n";
  outfile << "-- number of preemptions: " << s.preemptions_total << " (" << s.preemptions_cpu << "/" << s.preemptions_io << ")\n";
  outfile << "\n";
    // Close the output file
    outfile.close();
    
  return s;
}
Simulation simulateRR(vector<Process> & processes, int cst, int tslice) { //cst is the context switch time
  cout << "time 0ms: Simulator started for RR [Q <empty>]\n";
  size_t terminated_count = 0;
  list<int> queue; 
  unordered_map<int, set<Integer> > map;
  Simulation s; 
  bool cpu_available = true;
  int cpu_start_time = 0;
  int time = 1, sliceTime = 1; 
  int cpu_user = -1;
  vector<int> indices(processes.size(), 0);
  for (size_t i = 0; i < processes.size(); i++) { 
      map[processes[i].arrival_time].insert(Integer(i, processes));
      processes[i].resetProcess();
  }
  for (;;time++, sliceTime++) { 
    // -1: make cpu_available true, 0: arrival, 1: cpu start, 2: cpu end, 3: tau recalc, 4: preemption, 5: I/O start, 6: I/O end, 7: process finish, 8: cpu becomes available, 9: waiting in the queue
    if (terminated_count == processes.size() || map.empty()) break;

    auto it = map.find(time);

    if (it != map.end()) { 
      set<Integer> sorting_set;
      for (auto it2 : it->second) { 
        sorting_set.insert(it2);
      }
      it->second = sorting_set;
    }
    if (sliceTime == tslice && cpu_available == false && processes[cpu_user].remaining_time > tslice && cpu_start_time != time) { 
      if (queue.size() == 0) { 
        if (time < 10000)
            cout << "time " << time << "ms: Time slice expired; no preemption because ready queue is empty [Q <empty>]" << endl;
        if (!cpu_available) { 
          map[processes[cpu_user].remaining_time + time - tslice].erase(Integer(processes[cpu_user].process_id - 'A', processes));
          processes[cpu_user].remaining_time -= sliceTime;
          map[processes[cpu_user].remaining_time + time].insert(Integer(processes[cpu_user].process_id - 'A', processes));
        }
      }
      else { 
        if (time < 10000)
            cout << "time " << time << "ms: Time slice expired; preempting process " << processes[cpu_user].process_id << " with " << processes[cpu_user].remaining_time - sliceTime << "ms remaining ";
        map[processes[cpu_user].remaining_time + time - tslice].erase(Integer(processes[cpu_user].process_id - 'A', processes));
        processes[cpu_user].remaining_time -= sliceTime;
        if (time < 10000)
            printQueue(queue, processes);
        map[time + cst / 2].insert(Integer(processes[cpu_user].process_id - 'A', processes));
        processes[cpu_user].indicator = 4;
      }
      sliceTime = 0;
    }
    while (it != map.end() && !it->second.empty()) {
      Integer temp_int = *map[time].begin(); 
      Process & p = processes[temp_int.index];
      // cout << temp_int.index << endl;
      map[time].erase(map[time].begin());
      if (p.indicator == 0) { 
        if (time < 10000)
            cout << "time " << p.arrival_time << "ms: Process " << p.process_id << " arrived; added to ready queue ";
        queue.push_back(p.process_id - 'A');
        p.time_queue = time;
        if (time < 10000)
            printQueue(queue, processes);
        p.indicator = 1;
        p.remaining_time = p.times[0].first;
      }
      else if (p.indicator == 1) { 
        sliceTime = 0;
        int process_time = p.times[indices[p.process_id - 'A']].first;
        if (p.remaining_time > 0 && p.remaining_time < process_time) { 
          if (time < 10000)
            cout << "time " << time << "ms: Process " << p.process_id << " started using the CPU for remaining " << p.remaining_time << "ms of " << process_time << "ms burst ";
        }
        else { 
          if (time < 10000)
            cout << "time " << time << "ms: Process " << p.process_id << " started using the CPU for " << process_time << "ms burst ";
        }
        if (time < 10000)
            printQueue(queue, processes);
        map[time + p.remaining_time].insert(Integer(temp_int.index, processes));
        p.indicator = 2;
        cpu_available = false;
        cpu_start_time = time;
        cpu_user = p.process_id - 'A';
      }
      else if (p.indicator == 2) { 
        int bursts_left = p.times.size() - indices[p.process_id - 'A'] - 1;
        if (bursts_left == 0) { 
          cout << "time " << time << "ms: Process " << p.process_id << " terminated ";
          printQueue(queue, processes);
          p.indicator = 7;
          terminated_count++;
        }
        else { 
          int io_time = p.times[indices[p.process_id - 'A']].second;
          if (bursts_left > 1) {
            if (time < 10000)
                cout << "time " << time << "ms: Process " << p.process_id << " completed a CPU burst; " << bursts_left << " bursts to go ";
          }
          else {
            if (time < 10000)
                cout << "time " << time << "ms: Process " << p.process_id << " completed a CPU burst; 1 burst to go ";
          }
          if (time < 10000)
            printQueue(queue, processes);
          if (time < 10000)
            cout << "time " << time << "ms: Process " << p.process_id << " switching out of CPU; blocking on I/O until time " << time + io_time + cst / 2 << "ms ";
          // p.indicator = 6;
          if (time < 10000)
            printQueue(queue, processes);
          map[time + io_time + cst / 2].insert(Integer(temp_int.index, processes));
        }
        sliceTime = 0;
        p.indicator = -1;
        map[time + cst / 2].insert(Integer(temp_int.index, processes));
        // cpu_available = true;
      }
      else if (p.indicator == -1) { 
        cpu_available = true;
        p.indicator = 6;
      }
      else if (p.indicator == 4) {         
        queue.push_back(p.process_id - 'A');
        p.time_queue = time;
        p.indicator = 1;
        //remove the ending of this process from the map
        // map[time + p.remaining_time].erase(Integer(p.process_id - 'A', processes));
        // if (map[time + p.remaining_time].empty()) { 
        //   map.erase(time + p.remaining_time);
        // }
        cpu_available = true;
      }
      else if (p.indicator == 6) { 
        if (time < 10000)
            cout << "time " << time << "ms: Process " << p.process_id << " completed I/O; added to ready queue ";
        queue.push_back(p.process_id - 'A');
        p.time_queue = time;
        if (time < 10000)
            printQueue(queue, processes);
        p.indicator = 1;
        indices[p.process_id - 'A']++;
        p.remaining_time = p.times[indices[p.process_id - 'A']].first;
      }

    }


    if (cpu_available && !queue.empty()) { 
      Process & p = processes[*queue.begin()];
      queue.pop_front();
      // cout << p.process_id << endl;
      map[time + cst / 2].insert(Integer(p.process_id - 'A', processes));
      p.indicator = 1;
      p.wait_time += time - p.time_queue;
      // cout << "something" << endl;
      cpu_available = false;
      cpu_start_time = time + cst / 2;
    }
  }
  cout << "time " << time - 1 + cst / 2 << "ms: Simulator ended for RR [Q <empty>]" << endl;

  s.wait_count_io = 0;
  s.wait_count_cpu = 0;
  s.wait_count_total = 0;
  for (Process & p : processes) { 
      if (p.cpu_bound) { 
          s.wait_time_cpu += p.wait_time;
          s.wait_count_cpu += p.cpu_bursts;
      }
      else { 
          s.wait_time_io += p.wait_time;
          s.wait_count_io += p.cpu_bursts;
      }
      s.wait_count_total += p.cpu_bursts;
      s.wait_time_total += p.wait_time;
  }
  s.cpu_burst_cpu = ((double) CPU_TIME_CPU) / s.wait_count_cpu;
  s.cpu_burst_io = ((double) CPU_TIME_IO) / s.wait_count_io;
  s.cpu_burst_total = (double) ((double) CPU_TIME_IO + (double) CPU_TIME_CPU) / (s.wait_count_io + s.wait_count_cpu);

  s.turnaround_cpu = s.cpu_burst_cpu + (double) ((double) (s.wait_time_cpu + s.preemptions_cpu * cst) / (double) s.wait_count_cpu) + cst;
  s.turnaround_io = s.cpu_burst_io + (double) ((double) (s.wait_time_io + s.preemptions_io * cst) / (double) s.wait_count_io) + cst;
  s.turnaround_total = s.cpu_burst_total + (double) ((double) (s.wait_time_total + s.preemptions_total * cst) / (double) s.wait_count_total) + cst;

  ofstream outfile("simout.txt", std::ios_base::app);
    // Write the output to the file
    outfile << fixed << setprecision(3);
  outfile << "Algorithm RR\n";
  outfile << "-- CPU utilization: ";
  printDouble((double) (((double) CPU_TIME_CPU + CPU_TIME_IO) / (double) time) * 100, outfile);
  outfile << "%\n";
  outfile << "-- average CPU burst time: ";
  printDouble(s.cpu_burst_total, outfile);
  outfile << " ms (";
  printDouble(s.cpu_burst_cpu, outfile);
  outfile << " ms/";
  printDouble(s.cpu_burst_io, outfile);
  outfile << " ms)\n";
  outfile << "-- average wait time: ";
  printDouble((double) ((double) s.wait_time_total / (double) s.wait_count_total), outfile);
  outfile << " ms (";
  printDouble((double) ((double) s.wait_time_cpu / (double) s.wait_count_cpu), outfile);
  outfile<< " ms/";
  printDouble((double) s.wait_time_io / (double) s.wait_count_io, outfile);
  outfile << " ms)\n";
  outfile << "-- average turnaround time: ";
  printDouble(s.turnaround_total, outfile);
  outfile << " ms (";
  printDouble(s.turnaround_cpu, outfile);
  outfile << " ms/";
  printDouble(s.turnaround_io, outfile);
  outfile << " ms)\n";

  s.wait_count_cpu += s.preemptions_cpu;
  s.wait_count_io += s.preemptions_io;
  s.wait_count_total += s.preemptions_total;

  outfile << "-- number of context switches: " << s.wait_count_total << " (" << s.wait_count_cpu << "/" << s.wait_count_io << ")\n";
  outfile << "-- number of preemptions: " << s.preemptions_total << " (" << s.preemptions_cpu << "/" << s.preemptions_io << ")";
  // outfile << "\n";
    // Close the output file
    outfile.close();
    

  return s;
}

int main(int argc, char ** argv) {
    int n, ncpu, seed;
    if (argc != 9) { 
        cerr << "ERROR: INVALID COMMAND LINE ARGUMENTS" << endl;
        return 1; 
    }
    n = atoi(argv[1]);
    if (n <= 0 || n > 26) { 
        cerr << "ERROR: INVALID NUMBER OF PROCESSES" << endl;
        return 1; 
    }
    ncpu = atoi(argv[2]);
    if (ncpu < 0 || ncpu > n) { 
        cerr << "ERROR: INVALID NUMBER OF CPU-BURST PROCESSES" << endl;
        return 1; 
    }
    seed = atoi(argv[3]);
    lambda = (double) atof(argv[4]);
    if (lambda < 0 || lambda >= 1) { 
        cerr << "ERROR: INVALID LAMBDA VALUE" << endl;
        return 1;
    }
    BOUND = atoi(argv[5]);
    srand48(seed);
    int cst = atoi(argv[6]);
    if (cst < 0) { 
        cerr << "ERROR: INVALID CONTEXT SWITCH TIME" << endl;
        return 1;
    }
    float alpha = atof(argv[7]);
    if (alpha < 0 || alpha > 1) { 
        cerr << "ERROR: INVALID ALPHA VALUE" << endl;
        return 1;
    }
    int tslice = atoi(argv[8]);
    if (tslice < 0) { 
        cerr << "ERROR: INVALID TIME SLICE VALUE" << endl;
        return 1;
    }
    // Command line arguments are dealt at this point

    vector<Process> processes;
    char c = 'A';
    for (int i = 0; i < n; i++) { 
        processes.push_back(Process(c + i, i >= n - ncpu));
    }
    if (ncpu == 1) { 
        cout << "<<< PROJECT PART I -- process set (n=" << n << ") with " << ncpu << " CPU-bound process >>>" << endl;    
    }
    else {
        cout << "<<< PROJECT PART I -- process set (n=" << n << ") with " << ncpu << " CPU-bound processes >>>" << endl;
    }
    for (auto p : processes) { 
         p.printProcessSummarized();
    }
    cout << endl;   
    cout << fixed << setprecision(2) << "<<< PROJECT PART II -- t_cs=" << cst <<"ms; alpha=" << alpha << "; t_slice=" << tslice << "ms >>>" << endl; 
    simulateFCFS(processes, cst);
    simulateSJF(processes, cst, alpha);
    simulateSRT(processes, cst, alpha);
    simulateRR(processes, cst, tslice);


    return 0;
}