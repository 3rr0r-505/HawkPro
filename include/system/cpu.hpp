#pragma once
#include "types.hpp"
#include <string>

// Reads total CPU snapshot (total + idle + user + system)
CpuSnapshot read_cpu_snapshot();

// Calculates total CPU usage percentage using previous snapshot
double calculate_cpu_usage(const CpuSnapshot& prev, const CpuSnapshot& curr);

// Calculates CPU breakdown percentages
double calculate_cpu_user_percent(const CpuSnapshot& prev, const CpuSnapshot& curr);
double calculate_cpu_system_percent(const CpuSnapshot& prev, const CpuSnapshot& curr);
double calculate_cpu_idle_percent(const CpuSnapshot& prev, const CpuSnapshot& curr);

// Returns CPU model name string
std::string get_cpu_model();
