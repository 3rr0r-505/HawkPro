#include "process/process_reader.hpp"
#include <vector>
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <pwd.h>
#include <unistd.h>
#include <iomanip>

namespace fs = std::filesystem;

// Convert UID to username
static std::string uid_to_user(uid_t uid) {
    passwd* pw = getpwuid(uid);
    return pw ? pw->pw_name : "unknown";
}

// Convert clock ticks to HH:MM:SS string
static std::string ticks_to_timestr(uint64_t ticks) {
    long ticks_per_sec = sysconf(_SC_CLK_TCK);
    uint64_t total_sec = ticks / ticks_per_sec;
    uint64_t hours   = total_sec / 3600;
    uint64_t minutes = (total_sec % 3600) / 60;
    uint64_t seconds = total_sec % 60;

    std::ostringstream oss;
    oss << std::setw(2) << std::setfill('0') << hours
        << ":" << std::setw(2) << std::setfill('0') << minutes
        << ":" << std::setw(2) << std::setfill('0') << seconds;
    return oss.str();
}

std::vector<ProcessInfo>
read_processes(std::unordered_map<int, uint64_t>& prev_cpu_ticks,
               uint64_t total_cpu_delta) {

    std::vector<ProcessInfo> list;

    for (const auto& dir : fs::directory_iterator("/proc")) {
        if (!dir.is_directory()) continue;

        std::string pid_str = dir.path().filename();
        if (!std::all_of(pid_str.begin(), pid_str.end(), ::isdigit))
            continue;

        int pid = std::stoi(pid_str);

        // ---- /proc/[pid]/stat ----
        std::ifstream statf(dir.path() / "stat");
        if (!statf) continue;

        ProcessInfo p{};
        p.pid = pid;

        std::string comm;
        uint64_t utime = 0, stime = 0;
        statf >> p.pid >> comm >> p.state;
        for (int i = 0; i < 11; ++i) statf.ignore(1024, ' ');
        statf >> utime >> stime;

        uint64_t total_ticks = utime + stime;
        uint64_t prev = prev_cpu_ticks[pid];
        prev_cpu_ticks[pid] = total_ticks;

        if (total_cpu_delta > 0)
            p.cpu_percent = 100.0 * (total_ticks - prev) / total_cpu_delta;

        // Fill time string
        p.time = ticks_to_timestr(total_ticks);

        // ---- /proc/[pid]/status ----
        std::ifstream statusf(dir.path() / "status");
        std::string line;
        uid_t uid = 0;

        while (std::getline(statusf, line)) {
            std::istringstream iss(line);
            std::string key;
            iss >> key;
            if (key == "Uid:") {
                iss >> uid;
            } else if (key == "VmRSS:") {
                uint64_t kb = 0;
                iss >> kb;
                p.mem_kb = kb;
            }
        }
        p.user = uid_to_user(uid);

        // ---- /proc/[pid]/cmdline ----
        std::ifstream cmdf(dir.path() / "cmdline");
        std::getline(cmdf, p.command, '\0'); // read null-terminated command
        if (p.command.empty()) p.command = comm;

        list.push_back(p);
    }

    return list;
}
