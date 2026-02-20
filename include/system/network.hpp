#pragma once
#include <cstdint>
#include <string>
#include "types.hpp" 

// Read total RX/TX bytes (all non-loopback interfaces)
NetworkSnapshot read_network_snapshot();

// Calculate Mbps using previous snapshot and interval (ms)
double calculate_network_mbps(const NetworkSnapshot& prev,
                                const NetworkSnapshot& curr,
                                int interval_ms);

// Get primary non-loopback IPv4 address
std::string get_ip_address();
