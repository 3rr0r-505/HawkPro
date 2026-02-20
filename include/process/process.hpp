#pragma once
#include "types.hpp"
#include <unordered_map>
#include <vector> 

class ProcessTable {
public:
    void update();
    const std::vector<ProcessInfo>& list() const;

    void sort_by_cpu(bool desc = true);
    void sort_by_mem(bool desc = true);
    void sort_by_user(bool desc = true);

private:
    std::vector<ProcessInfo> processes;
};
