#pragma once
#include "types.hpp"
#include <unordered_map>
#include <vector>

std::vector<ProcessInfo>
read_processes(std::unordered_map<int, uint64_t>& prev_cpu_ticks,
                uint64_t total_cpu_delta);
