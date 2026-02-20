#include "system/uptime.hpp"
#include <fstream>
#include <sstream>

std::string get_uptime_string() {
    std::ifstream file("/proc/uptime");
    if (!file.is_open())
        return "Unknown";

    double seconds = 0.0;
    file >> seconds;

    long secs = static_cast<long>(seconds);
    int days = secs / 86400;
    secs %= 86400;
    int hours = secs / 3600;
    secs %= 3600;
    int mins = secs / 60;

    std::ostringstream out;
    if (days > 0)
        out << days << "d ";
    out << hours << "h " << mins << "m";

    return out.str();
}
