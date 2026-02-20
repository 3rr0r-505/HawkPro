#pragma once
#include <cstdint>
#include "types.hpp" 

// Reads disk info for root filesystem (/)  
DiskInfo read_disk_info();

// Returns disk device name + model
std::string get_disk_model();
