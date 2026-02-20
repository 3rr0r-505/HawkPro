#pragma once
#include "types.hpp"

// Reads memory info (total, used, free in KB)
MemoryInfo read_memory_info();

// Returns total memory usage percentage
double memory_usage_percent(const MemoryInfo& mem);

// Returns RAM manufacturer string
std::string get_ram_manufacturer();
