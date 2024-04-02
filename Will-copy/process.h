#ifndef __PROCESS_H__
#define __PROCESS_H__
#include<string>
#include<vector>
#include<cmath>
class Process {
    public:
   Process(){}
   Process(const char _PID, const std::string& _boundType, int _arrivalTime,
                unsigned int _numCPUBursts,  int _tau)
    : PID(_PID),
      boundType(_boundType),
      arrivalTime(_arrivalTime),
      arrivalStatus(0),
      numCPUBursts(_numCPUBursts),
      CPUBurstTimes(std::vector<int>()),
      IOBurstTimes(std::vector<int>()),
      tau(_tau),
      currentBurst(0),
      status("Not Arrived"),
      blockTime(0), 
      remainingBurstTime(0)
       {}

    //getters
    char getPID() const {return PID;}
    unsigned int getNumCPUBursts() const {return numCPUBursts;}
    int getCPUBurstTime() const{return CPUBurstTimes[currentBurst];}
    int getIOBurstTime() const{return IOBurstTimes[currentBurst];}
    int getCurrentBurst() const{return currentBurst;}
    std::string getBoundType() const {return boundType;}
    int getArrivalTime() const {return arrivalTime;}
    int blockedUntil() const{return blockTime;}
    int getRemainingBurstTime() const {return remainingBurstTime;}
    int getTimeElapsed() const {return this->getCPUBurstTime() - this->getRemainingBurstTime();}
    int getTau() const {return tau;}
    std::string getStatus() const {return status;}
    int getArrivalStatus() const {return arrivalStatus;}





    //modifiers
    void setPID(const char _PID) {PID = _PID;}
    void setBoundType(const std::string& _boundType) {boundType = _boundType;}
    void setArrivalTime(unsigned int _arrivalTime) {arrivalTime = _arrivalTime;}
    void setNumCPUBursts(unsigned int _numCPUBursts) {numCPUBursts = _numCPUBursts;}
    void setBlock(int _blockTime) {blockTime = _blockTime;}
    void setStatus(const std::string& newStatus){status = newStatus;}
    void addCPUBurst(int burstTime){CPUBurstTimes.push_back(burstTime);}
    void addIOBurst(int burstTime){IOBurstTimes.push_back(burstTime);}
    void incrementNumBurst() {++currentBurst;}
    void recalculateTau(int newBurst);
    void setRemainingBurstTime(int time) { remainingBurstTime = time;}
    void decrementRemainingBurstTime() { --remainingBurstTime;}
    void setArrivalStatus(int newStatus) {arrivalStatus = newStatus;} 
    
    //operator overloads
    bool operator>(const Process& rhs) const;

    static float alpha;

    private:
        char PID;
        std::string boundType;
        int arrivalTime;
        int arrivalStatus;
        unsigned int numCPUBursts;
        std::vector<int> CPUBurstTimes;
        std::vector<int> IOBurstTimes;
        int tau;
        int currentBurst;
        std::string status;
        int blockTime;
        int remainingBurstTime;

        bool FCFS_Comparator(const Process& other) const;
        bool SJF_Comparator(const Process& other) const;
        bool SRT_Comparator(const Process& other) const;

};


#endif
