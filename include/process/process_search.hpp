#pragma once
#include <string>
#include <vector>
#include "process.hpp"

class ProcessSearch {
public:
    ProcessSearch() = default;

    // Set the process list to search through
    void set_process_list(const std::vector<ProcessInfo>& processes);

    // Search for first process containing 'query' (case-insensitive)
    // Returns the index in the vector, or -1 if not found
    int search(const std::string& query) const;

private:
    std::vector<ProcessInfo> processes;

    // helper for case-insensitive comparison
    static bool contains_case_insensitive(const std::string& str, const std::string& query);
};
