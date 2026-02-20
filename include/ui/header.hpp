#pragma once
#include <string>
#include "../types.hpp"
#include "layout.hpp"

namespace ui {

class Header {
public:
    Header(const Layout& layout);

    // Section 1 (top summary)
    void draw_section1(
        const std::string& os_name,
        const std::string& uptime,
        const std::string& cpu_model,
        const std::string& ram_manufacturer,
        const std::string& disk_model,
        double cpu_total,
        double cpu_user,
        double cpu_sys,
        double cpu_idle,
        const MemoryInfo& mem,
        const DiskInfo& disk,
        double net_mbps,
        uint64_t tasks,
        const std::string& ip
    ) const;

    // Section 2: Process table header
    void draw_process_table_header(const std::string& sort_col, bool ascending) const;

private:
    const Layout& layout;

    void draw_sort_triangle(int y, int x, bool asc) const;
};

} // namespace ui
