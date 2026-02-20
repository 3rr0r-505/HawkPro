#include "process/process.hpp"
#include "process/process_reader.hpp"
#include "system/cpu.hpp"
#include "system/memory.hpp"
#include <algorithm>

void ProcessTable::update() {
    static std::unordered_map<int, uint64_t> prev_cpu_ticks;
    static CpuSnapshot prev_cpu = read_cpu_snapshot();

    CpuSnapshot curr_cpu = read_cpu_snapshot();
    uint64_t total_delta = curr_cpu.total - prev_cpu.total;
    prev_cpu = curr_cpu;

    processes = read_processes(prev_cpu_ticks, total_delta);

    // Fix memory percentage properly using raw mem_kb
    MemoryInfo mem = read_memory_info();
    for (auto& p : processes) {
        if (mem.total_kb > 0)
            p.mem_percent = 100.0 * p.mem_kb / mem.total_kb;
    }
}

const std::vector<ProcessInfo>& ProcessTable::list() const {
    return processes;
}

void ProcessTable::sort_by_cpu(bool desc) {
    std::sort(processes.begin(), processes.end(),
        [desc](const ProcessInfo& a, const ProcessInfo& b) {
            return desc ? a.cpu_percent > b.cpu_percent
                        : a.cpu_percent < b.cpu_percent;
        });
}

void ProcessTable::sort_by_mem(bool desc) {
    std::sort(processes.begin(), processes.end(),
        [desc](const ProcessInfo& a, const ProcessInfo& b) {
            return desc ? a.mem_percent > b.mem_percent
                        : a.mem_percent < b.mem_percent;
        });
}

void ProcessTable::sort_by_user(bool desc) {
    std::sort(processes.begin(), processes.end(),
        [desc](const ProcessInfo& a, const ProcessInfo& b) {
            return desc ? a.user < b.user
                        : a.user > b.user;
        });
}
