#include "process/process_search.hpp"
#include <algorithm>
#include <cctype>

void ProcessSearch::set_process_list(const std::vector<ProcessInfo>& procs) {
    processes = procs;
}

int ProcessSearch::search(const std::string& query) const {
    if (query.empty()) return -1;

    for (size_t i = 0; i < processes.size(); ++i) {
        if (contains_case_insensitive(processes[i].command, query))
            return static_cast<int>(i);
    }
    return -1;
}

bool ProcessSearch::contains_case_insensitive(const std::string& str, const std::string& query) {
    if (query.size() > str.size()) return false;

    auto it = std::search(
        str.begin(), str.end(),
        query.begin(), query.end(),
        [](char c1, char c2) { return std::tolower(c1) == std::tolower(c2); }
    );
    return it != str.end();
}
