#include "system/os.hpp"
#include <fstream>
#include <string>

std::string get_os_name() {
    std::ifstream file("/etc/os-release");
    std::string line;

    if (!file.is_open())
        return "Unknown OS";

    while (std::getline(file, line)) {
        // We want PRETTY_NAME="Ubuntu 24.04 LTS"
        if (line.rfind("PRETTY_NAME=", 0) == 0) {
            auto value = line.substr(13); // skip PRETTY_NAME=
            if (!value.empty() && value.front() == '"')
                value.erase(0, 1);
            if (!value.empty() && value.back() == '"')
                value.pop_back();
            return value;
        }
    }

    return "Unknown OS";
}
