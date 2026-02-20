#include "system/tasks.hpp"
#include <filesystem>
#include <algorithm>

namespace fs = std::filesystem;

uint64_t get_total_tasks() {
    uint64_t count = 0;

    for (const auto& dir : fs::directory_iterator("/proc")) {
        if (!dir.is_directory()) continue;

        std::string name = dir.path().filename();
        if (std::all_of(name.begin(), name.end(), ::isdigit)) {
            ++count;
        }
    }

    return count;
}
