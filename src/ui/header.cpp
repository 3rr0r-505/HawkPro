#include <ncurses.h>
#include <iomanip>
#include <string>
#include "ui/header.hpp"

namespace ui {

Header::Header(const Layout& lay) : layout(lay) {}

void Header::draw_section1(
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
) const {
    auto sec1 = layout.get_sec1();
    int left   = sec1.x + 2;
    int right  = sec1.x + sec1.width - 2;
    int center = sec1.x + sec1.width / 2;
    int y = sec1.y + 1;

    // Top row: OS | HawkPro | Uptime
    mvprintw(y, left, "%s", os_name.c_str());
    mvprintw(y, center - 11, "HawkPro");
    std::string up = "Uptime: " + uptime;
    mvprintw(y, right - up.length(), "%s", up.c_str());

    // Column positions for other rows
    int col1 = left;
    int col2 = left + sec1.width / 4;
    int col3 = left + sec1.width / 2;
    int col4 = left + (sec1.width * 3) / 4;

    // Row 1: CPU / RAM / Disk
    mvprintw(y + 2, col1, "CPU Info: %s", cpu_model.c_str());
    mvprintw(y + 2, col2, "CPU Usage: %.2f %%", cpu_total);
    mvprintw(y + 2, col3, "Memory Usage: %.2f %%", 
             (mem.total_kb ? 100.0 * mem.used_kb / mem.total_kb : 0.0));
    mvprintw(y + 2, col4, "Total Disk: %.2f GB", static_cast<double>(disk.total_gb));

    // Row 2: Memory info
    mvprintw(y + 3, col1, "Memory Info: %s", ram_manufacturer.c_str());
    mvprintw(y + 3, col2, "Sys: %.2f %%", cpu_sys);
    mvprintw(y + 3, col3, "Size: %.2f GB", mem.total_kb / 1024.0 / 1024.0);
    mvprintw(y + 3, col4, "Available: %.2f GB", static_cast<double>(disk.free_gb));

    // Row 3: Disk info
    mvprintw(y + 4, col1, "Disk Info: %s", disk_model.c_str());
    mvprintw(y + 4, col2, "User: %.2f %%", cpu_user);
    mvprintw(y + 4, col3, "Used: %.2f GB", mem.used_kb / 1024.0 / 1024.0);
    mvprintw(y + 4, col4, "Network: %.2f Mbps", net_mbps);

    // Row 4: Tasks / Idle / Free / IP
    mvprintw(y + 5, col1, "#Tasks: %lu", tasks);
    mvprintw(y + 5, col2, "Idle: %.2f %%", cpu_idle);
    mvprintw(y + 5, col3, "Free: %.2f GB", mem.free_kb / 1024.0 / 1024.0);
    mvprintw(y + 5, col4, "IP: %s", ip.c_str());
}

void Header::draw_sort_triangle(int y, int x, bool asc) const {
    mvaddch(y, x, asc ? ACS_UARROW : ACS_DARROW);
}

void Header::draw_process_table_header(const std::string& sort_col, bool ascending) const {
    auto sec2 = layout.get_sec2();
    int y = sec2.y + 1;
    int x = sec2.x + 2;

    // Column widths must match Table::draw_row
    constexpr int w_pid   = 8;
    constexpr int w_user  = 15;
    constexpr int w_cpu   = 7;
    constexpr int w_mem   = 7;
    constexpr int w_state = 3;
    constexpr int w_time  = 10;

    int col_pid   = x;
    int col_user  = col_pid + w_pid + 2;
    int col_cpu   = col_user + w_user + 2;
    int col_mem   = col_cpu + w_cpu + 2;
    int col_state = col_mem + w_mem + 2;
    int col_time  = col_state + w_state + 2;
    int col_cmd   = col_time + w_time + 2;

    // Print headers
    mvprintw(y, col_pid,   "%-*s", w_pid, "PID");
    mvprintw(y, col_user,  "%-*s", w_user, "USER");
    mvprintw(y, col_cpu,   "%*s",   w_cpu, "%CPU");  // right-align
    mvprintw(y, col_mem,   "%*s",   w_mem, "%MEM");  // right-align
    mvprintw(y, col_state, "%-*s", w_state, "S+");
    mvprintw(y, col_time,  "%-*s", w_time, "TIME+");
    mvprintw(y, col_cmd,   "COMMAND");

    // Draw sorting arrows
    // if (sort_col == "user") draw_sort_triangle(y, col_user + w_user - 1, ascending);
    // if (sort_col == "cpu")  draw_sort_triangle(y, col_cpu + w_cpu - 1, ascending);
    // if (sort_col == "mem")  draw_sort_triangle(y, col_mem + w_mem - 1, ascending);

    // Draw horizontal line under header
    mvhline(y + 1, sec2.x + 1, ACS_HLINE, sec2.width - 2);
}

} // namespace ui
