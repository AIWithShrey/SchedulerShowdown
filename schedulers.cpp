#include "schedulers.h"
#include <climits>


//Round Robin scheduler implementation. In general, this function maintains a double ended queue
//of processes that are candidates for scheduling (the ready variable) and always schedules
//the first process on that list, if available (i.e., if the list has members)
int RoundRobin(const int& curTime, const vector<Process>& procList, const int& timeQuantum)
{
    static int timeToNextSched = timeQuantum;  //keeps track of when we should actually schedule a new process
    static deque<int> ready;  //keeps track of the processes that are ready to be scheduled

    int idx = -1;

    // first look through the process list and find any processes that are newly ready and
    // add them to the back of the ready queue
    for(int i = 0, i_end = procList.size(); i < i_end; ++i)
    {
        if(procList[i].startTime == curTime)
        {
            ready.push_back(i);
        }
    }

    // now take a look the head of the ready queue, and update if needed
    // (i.e., if we are supposed to schedule now or the process is done)
    if(timeToNextSched == 0 || procList[ready[0]].isDone)
    {
        // the process at the start of the ready queue is being taken off of the
        // processor

        // if the process isn't done, add it to the back of the ready queue
        if(!procList[ready[0]].isDone)
        {
            ready.push_back(ready[0]);
        }

        // remove the process from the front of the ready queue and reset the time until
        // the next scheduling
        ready.pop_front();
        timeToNextSched = timeQuantum;
    }

    // if the ready queue has any processes on it
    if(ready.size() > 0)
    {
        // grab the front process and decrement the time to next scheduling
        idx = ready[0];
        --timeToNextSched;
    }
    // if the ready queue has no processes on it
    else
    {
        // send back an invalid process index and set the time to next scheduling
        // value so that we try again next time step
        idx = -1;
        timeToNextSched = 0;
    }

    // return back the index of the process to schedule next
    return idx;
}

//Shortest Process Next scheduler implementation. In general, this function maintains a double ended queue
//of processes that are candidates for scheduling (the ready variable) and always schedules
//the process with the shortest total time needed, if available (i.e., if the list has members)
int ShortestProcessNext(const int& curTime, vector<Process>& procList) {
    int shortestIndex = -1;
    int shortestTimeNeeded = INT_MAX;
    static int curRunning = -1;
    
    if ( curRunning == -1 || procList[curRunning].isDone)
    {
        for (size_t i = 0; i < procList.size(); i++) {
            if (procList[i].startTime <= curTime && !procList[i].isDone && procList[i].totalTimeNeeded < shortestTimeNeeded) {
                shortestIndex = i;
                shortestTimeNeeded = procList[i].totalTimeNeeded;
            }
        }
    }

    if(shortestIndex != -1) curRunning = shortestIndex;
    return curRunning;
}

//Shortest Remaining Time scheduler implementation. In general, this function maintains a double ended queue
//of processes that are candidates for scheduling (the ready variable) and always schedules
//the process with the shortest remaining time needed, if available (i.e., if the list has members)

// Calculate how much time is left before a process completes
int remainingTime(const vector<Process> &procList, const int &i) {
        return procList[i].totalTimeNeeded - procList[i].timeScheduled;
    }


int ShortestRemainingTime(const int& curTime, vector<Process>& procList)
{
    static vector<int> ready;

    int idx = -1;

    // Add newly ready processes to the deque of ready processes
    for (int i = 0, i_end = procList.size(); i < i_end; ++i) {
        if (procList[i].startTime == curTime) {
            ready.push_back(i);
        }
    }

    // Check if the currently running process is done
    if (procList[ready[0]].isDone) {
        ready.erase(ready.begin());
    }
    

    // While there are pending processes
    if (!ready.empty()) {
        // Find process with shortest remaining time left to completion
        int minIndex = 0, minRemainingTime = remainingTime(procList, ready[0]);
        for (int i = 1; i < static_cast<int>(ready.size()); i++) {
            int currentRemainingTime = remainingTime(procList, ready[i]);
            if (currentRemainingTime < minRemainingTime) {
                minRemainingTime = currentRemainingTime;
                minIndex = i;
            } else if (currentRemainingTime == minRemainingTime) {
                // Ensure in the case of a tie the process earlier in the procList
                // vector runs first
                minIndex = ready[i] < ready[minIndex] ? i : minIndex;
            }
        }
        // Move process with shortest remaining time to the front
        swap(ready[0], ready[minIndex]);
    }

    // If processes are being ran return the head of the deque
    if (!ready.empty()) {
        idx = ready[0];
    } else {
        idx = -1;
    }

    return idx;

}

int calculateWaitingTime(const vector<Process>& processes, int curTime, int index) {
    return curTime - processes[index].startTime - processes[index].timeScheduled;
}

float calculateResponseRatio(const vector<Process>& processes, int curTime, int index) {
    int waitingTime = calculateWaitingTime(processes, curTime, index);
    return (float)(waitingTime + processes[index].totalTimeNeeded) / (float)(processes[index].totalTimeNeeded);
}

int HighestResponseRatioNext(const int& currentTime, const vector<Process>& processList) {
    static vector<int> readyProcesses;

    int selectedProcessIndex = -1;

    for(size_t i = 0; i < processList.size(); i++) {
        if(processList[i].startTime == currentTime) {
            readyProcesses.push_back(i);
        }
    }

    if(processList[readyProcesses[0]].isDone) {
        readyProcesses.erase(readyProcesses.begin());

        if(!readyProcesses.empty()) {
            int highestRatioIndex = 0;
            float highestRatioValue = calculateResponseRatio(processList, currentTime, readyProcesses[0]);
            
            for(size_t i = 1; i < readyProcesses.size(); i++) {
                float currentRatio = calculateResponseRatio(processList, currentTime, readyProcesses[i]);
                if(currentRatio > highestRatioValue) {
                    highestRatioValue = currentRatio;
                    highestRatioIndex = i;
                } else if(currentRatio == highestRatioValue) {
                    highestRatioIndex = readyProcesses[i] < readyProcesses[highestRatioIndex] ? i : highestRatioIndex;
                }
            }

            swap(readyProcesses[0], readyProcesses[highestRatioIndex]);
        }
    }

    if(!readyProcesses.empty()) {
        selectedProcessIndex = readyProcesses[0];
    }

    return selectedProcessIndex;
}