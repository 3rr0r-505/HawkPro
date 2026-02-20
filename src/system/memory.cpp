#include "system/memory.hpp"
#include <fstream>
#include <string>

MemoryInfo read_memory_info() {
    std::ifstream file("/proc/meminfo");
    MemoryInfo mem{};

    if (!file.is_open())
        return mem;

    std::string key;
    uint64_t value;
    std::string unit;

    uint64_t mem_available = 0;
    uint64_t mem_buffers   = 0;
    uint64_t mem_cached    = 0;

    while (file >> key >> value >> unit) {
        if (key == "MemTotal:")      mem.total_kb     = value;
        else if (key == "MemAvailable:") mem_available = value;
        else if (key == "Buffers:")      mem_buffers   = value;
        else if (key == "Cached:")       mem_cached    = value;
    }

    // Used memory = total - available
    mem.used_kb = (mem.total_kb > mem_available) ? mem.total_kb - mem_available : 0;
    mem.free_kb = mem_available;  // MemAvailable gives more realistic free memory

    return mem;
}

double memory_usage_percent(const MemoryInfo& mem) {
    if (mem.total_kb == 0) return 0.0;
    return 100.0 * static_cast<double>(mem.used_kb) / mem.total_kb;
}

std::string get_ram_manufacturer() {
    std::ifstream file("/sys/devices/system/memory/memory0/device/vendor");
    if (!file.is_open()) return "Not Specified";

    std::string manufacturer;
    std::getline(file, manufacturer);
    return manufacturer.empty() ? "Not Specified" : manufacturer;
}
