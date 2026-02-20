#pragma once
#include <cstdint>
#include <algorithm>

// Returns total number of running processes (numeric directories in /proc)
uint64_t get_total_tasks();
