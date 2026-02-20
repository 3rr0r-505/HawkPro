#include "system/network.hpp"
#include <fstream>
#include <sstream>
#include <string>
#include <ifaddrs.h>
#include <arpa/inet.h>
#include <netinet/in.h>

NetworkSnapshot read_network_snapshot() {
    std::ifstream file("/proc/net/dev");
    NetworkSnapshot snap{};

    if (!file.is_open())
        return snap;

    std::string line;
    std::getline(file, line); // skip header
    std::getline(file, line); // skip header

    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string iface;
        uint64_t rx, tx;
        uint64_t dummy;

        iss >> iface;
        if (iface.find("lo:") != std::string::npos) continue;

        iss >> rx;
        for (int i = 0; i < 7; ++i) iss >> dummy;
        iss >> tx;

        snap.rx_bytes += rx;
        snap.tx_bytes += tx;
    }

    return snap;
}

double calculate_network_mbps(const NetworkSnapshot& prev,
                                const NetworkSnapshot& curr,
                                int interval_ms) {
    uint64_t delta_bytes =
        (curr.rx_bytes + curr.tx_bytes) -
        (prev.rx_bytes + prev.tx_bytes);

    if (interval_ms <= 0) return 0.0;

    double seconds = interval_ms / 1000.0;
    double bits = static_cast<double>(delta_bytes) * 8.0;

    return (bits / seconds) / (1024.0 * 1024.0); // Mbps
}

std::string get_ip_address() {
    struct ifaddrs* ifaddr;
    if (getifaddrs(&ifaddr) == -1) return "0.0.0.0";

    std::string ip = "0.0.0.0";
    for (struct ifaddrs* ifa = ifaddr; ifa != nullptr; ifa = ifa->ifa_next) {
        if (!ifa->ifa_addr) continue;
        if (ifa->ifa_addr->sa_family == AF_INET) {
            if (std::string(ifa->ifa_name) == "lo") continue;

            char buf[INET_ADDRSTRLEN];
            void* addr_ptr = &((struct sockaddr_in*)ifa->ifa_addr)->sin_addr;
            inet_ntop(AF_INET, addr_ptr, buf, INET_ADDRSTRLEN);
            ip = buf;
            break; // take first non-loopback IPv4
        }
    }

    freeifaddrs(ifaddr);
    return ip;
}
