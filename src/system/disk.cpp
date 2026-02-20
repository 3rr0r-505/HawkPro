#include "system/disk.hpp"
#include <sys/statvfs.h>
#include <fstream>
#include <string>

DiskInfo read_disk_info() {
    struct statvfs fs {};
    DiskInfo disk{};

    // Root filesystem
    if (statvfs("/", &fs) != 0)
        return disk;

    uint64_t block_size = fs.f_frsize;

    uint64_t total_bytes = fs.f_blocks * block_size;
    uint64_t free_bytes  = fs.f_bavail * block_size;

    disk.total_gb = total_bytes / (1024ULL * 1024 * 1024); // GB
    disk.free_gb  = free_bytes  / (1024ULL * 1024 * 1024); // GB

    return disk;
}

std::string get_disk_model() {
    std::ifstream file("/sys/block/sda/device/model");
    if (!file.is_open()) return "Unknown";
    std::string model;
    std::getline(file, model);
    return model;
}
