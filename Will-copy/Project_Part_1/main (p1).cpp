#include <cstdlib>
#include <cmath>
#include <string>
#include <ctime>
#include<vector>
#include<iostream>


double next_exp(double Lambda, int upperBound) {
    double nextExp = 0;
    do {
        double r = drand48();
        nextExp = -log(r) / Lambda;
    }while(nextExp > upperBound);
    
    return  nextExp;
}

int main(int argc, char* argv[]) {
    if(argc != 6) {
        std::cerr << "Usage: " << argv[0] << " numProcesses cpuBound seed lambda upperBound\n";
        return 1;
    }


    // CL Arguments
    int numProcesses = std::stoi(std::string(argv[1]));
    int cpuBound = std::stoi(std::string(argv[2]));
    int seed = std::stoi(std::string(argv[3]));
    double lambda = std::stod(std::string(argv[4]));
    int upperBound = std::stoi(std::string(argv[5]));


    // Seeding Random Number Generator
    srand48(seed);
    if(cpuBound > 1){
        std::cout << "<<< PROJECT PART I -- process set (n=" << numProcesses <<  ") with " << 
        cpuBound << " CPU-bound processes >>>" << std::endl;
    } else {
        std::cout << "<<< PROJECT PART I -- process set (n=" << numProcesses <<  ") with " << 
        cpuBound << " CPU-bound process >>>" << std::endl;
    }
    


    for(int i = 0; i < numProcesses; ++i) {
        char PID = 'A' + i;
        std::string boundType = i >= numProcesses - cpuBound ? "CPU" : "I/O";
        double arrivalTime = floor(next_exp(lambda, upperBound));
        unsigned int numCPUBursts = ceil(drand48() * 64);
        std::cout << boundType << "-bound process " << PID << ": arrival time "
            << arrivalTime << "ms; " << numCPUBursts << " CPU bursts:" << std::endl;
        for(unsigned int j = 0; j < numCPUBursts - 1; ++j){
            int cpuBurst = ceil(next_exp(lambda,upperBound));
            int ioBurst = ceil(next_exp(lambda, upperBound)) * 10;
            if(boundType == "CPU"){
                cpuBurst *= 4;
                ioBurst /= 8;
            }
            std::cout << "--> CPU burst " << cpuBurst << "ms --> I/O burst " << ioBurst << "ms" << std::endl;
        }
        int finalBurst =  boundType == "CPU" ? ceil(next_exp(lambda,upperBound)) * 4 : ceil(next_exp(lambda,upperBound));
        std::cout << "--> CPU burst " << finalBurst << "ms" << std::endl;
    }
        



    return 0;
}
