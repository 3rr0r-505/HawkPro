#include "system/cpu.hpp"
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <cctype>

CpuSnapshot read_cpu_snapshot() {
    std::ifstream file("/proc/stat");
    CpuSnapshot snap{};

    if (!file.is_open())
        return snap;

    std::string cpu;
    uint64_t user, nice, system, idle, iowait, irq, softirq, steal;

    // cpu  user nice system idle iowait irq softirq steal ...
    file >> cpu >> user >> nice >> system >> idle
            >> iowait >> irq >> softirq >> steal;

    snap.user   = user + nice;
    snap.system = system + irq + softirq;
    snap.idle   = idle + iowait;
    snap.total  = snap.user + snap.system + snap.idle + steal;

    return snap;
}

double calculate_cpu_usage(const CpuSnapshot& prev, const CpuSnapshot& curr) {
    uint64_t total_delta = curr.total - prev.total;
    uint64_t idle_delta  = curr.idle  - prev.idle;

    if (total_delta == 0) return 0.0;

    return 100.0 * (total_delta - idle_delta) / total_delta;
}

double calculate_cpu_user_percent(const CpuSnapshot& prev, const CpuSnapshot& curr) {
    uint64_t total_delta = curr.total - prev.total;
    uint64_t user_delta  = curr.user - prev.user;
    if (total_delta == 0) return 0.0;
    return 100.0 * user_delta / total_delta;
}

double calculate_cpu_system_percent(const CpuSnapshot& prev, const CpuSnapshot& curr) {
    uint64_t total_delta = curr.total - prev.total;
    uint64_t sys_delta   = curr.system - prev.system;
    if (total_delta == 0) return 0.0;
    return 100.0 * sys_delta / total_delta;
}

double calculate_cpu_idle_percent(const CpuSnapshot& prev, const CpuSnapshot& curr) {
    uint64_t total_delta = curr.total - prev.total;
    uint64_t idle_delta  = curr.idle - prev.idle;
    if (total_delta == 0) return 0.0;
    return 100.0 * idle_delta / total_delta;
}

static std::string trim(const std::string& s) {
    size_t start = s.find_first_not_of(" \t");
    size_t end = s.find_last_not_of(" \t");
    if (start == std::string::npos) return "";
    return s.substr(start, end - start + 1);
}

static void normalize_spaces(std::string& s) {
    std::string result;
    bool space = false;
    for (char c : s) {
        if (std::isspace(c)) {
            if (!space) result += ' ';
            space = true;
        } else {
            result += c;
            space = false;
        }
    }
    s = trim(result);
}

static void remove_tokens(std::string& s) {
    const std::vector<std::string> tokens = {"(R)", "(TM)", "(tm)", "(r)"};
    for (const auto& t : tokens) {
        size_t pos;
        while ((pos = s.find(t)) != std::string::npos)
            s.erase(pos, t.length());
    }
}

std::string get_cpu_model() {
    std::ifstream file("/proc/cpuinfo");
    if (!file.is_open()) return "Unknown";

    std::string line;
    while (std::getline(file, line)) {
        if (line.find("model name") != std::string::npos) {
            auto pos = line.find(':');
            if (pos == std::string::npos) continue;

            std::string model = trim(line.substr(pos + 1));
            remove_tokens(model);
            normalize_spaces(model);

            std::istringstream iss(model);
            std::string word, vendor, series;

            if (iss >> vendor) {                 // First word → Intel / AMD
                while (iss >> word) {
                    if (word.find('i') == 0 ||   // i3, i5, i7, i9
                        word.find("Ryzen") != std::string::npos) {
                        series = word;
                        break;
                    }
                }
            }

            if (!vendor.empty() && !series.empty())
                return vendor + " " + series;

            return vendor; // fallback
        }
    }
    return "Unknown";
}

