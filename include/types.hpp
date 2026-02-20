#pragma once

#include <string>
#include <cstdint>

// CPU
struct CpuSnapshot {
    uint64_t total{0};
    uint64_t idle{0};
    uint64_t user{0};    // added for CPU user % calculation
    uint64_t system{0};  // added for CPU system % calculation
};

// Memory
struct MemoryInfo {
    uint64_t total_kb{0};
    uint64_t used_kb{0};
    uint64_t free_kb{0};
};

// Disk
struct DiskInfo {
    uint64_t total_gb{0};
    uint64_t free_gb{0};
};

// Network
struct NetworkSnapshot {
    uint64_t rx_bytes{0};
    uint64_t tx_bytes{0};
};

// Processes
struct ProcessInfo {
    int pid{0};
    std::string user;
    double cpu_percent{0.0};
    double mem_percent{0.0};   // calculated later in ProcessTable::update()
    uint64_t mem_kb{0};        // raw memory in KB from /proc/[pid]/status
    char state{'?'};           // process state
    std::string time;          // cumulative CPU time
    std::string command;       // command line
};

