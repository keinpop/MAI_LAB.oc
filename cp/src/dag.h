#pragma once

#include <iostream>
#include <vector>
#include <queue>
#include <unordered_set>
#include <algorithm>
#include <sys/wait.h>
#include <unistd.h>
#include <mutex>
#include <shared_mutex>

#include "./parserIni.h"

typedef struct _job_unit {
    Job j;
    std::vector<int> dep;
} UJob;

class DAG_J 
{
public:
    ParserIni _jobs;
    
    DAG_J() = default;

    ~DAG_J() = default;

    void createDAG(std::string fileName);
    void printDAG();

    void runCompile(const std::string & mtxNameLock);
    

private:
    bool hasCycle();
    bool hasCycleUtil(int v, std::vector<bool>& visited, std::vector<bool>& recStack);
    void executeJob(int jobIndex, std::string mtxNameLock);

private:
    std::vector<UJob> _graph;
    std::unordered_map<std::string, std::mutex> _mutexes;
};