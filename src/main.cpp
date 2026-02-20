#include <vector>
#include <thread>
#include <chrono>

#include "system/os.hpp"
#include "system/uptime.hpp"
#include "system/cpu.hpp"
#include "system/memory.hpp"
#include "system/disk.hpp"
#include "system/network.hpp"
#include "system/tasks.hpp"
#include "config.hpp"

#include "process/process.hpp"

#include "ui/screen.hpp"
#include "ui/layout.hpp"
#include "ui/header.hpp"
#include "ui/table.hpp"
#include "ui/input.hpp"

int main() {
    ui::Screen screen;
    ui::Layout layout(screen);
    ui::Header header(layout);
    ui::Table table(layout);

    static std::string current_sort = "cpu";
    static bool desc = true;                  
    ProcessTable proc_table;
    
    ui::InputHandler input(table, proc_table, current_sort, desc);

    CpuSnapshot prev_cpu = read_cpu_snapshot();
    NetworkSnapshot prev_net = read_network_snapshot();

    bool running = true;

    while (running) {
        std::this_thread::sleep_for(
            std::chrono::milliseconds(REFRESH_INTERVAL_MS)
        );

        // ---- SYSTEM SNAPSHOTS ----
        CpuSnapshot curr_cpu = read_cpu_snapshot();
        NetworkSnapshot curr_net = read_network_snapshot();

        double cpu_total = calculate_cpu_usage(prev_cpu, curr_cpu);
        double cpu_user  = calculate_cpu_user_percent(prev_cpu, curr_cpu);
        double cpu_sys   = calculate_cpu_system_percent(prev_cpu, curr_cpu);
        double cpu_idle  = calculate_cpu_idle_percent(prev_cpu, curr_cpu);

        double net_mbps  = calculate_network_mbps(
            prev_net, curr_net, REFRESH_INTERVAL_MS
        );

        MemoryInfo mem   = read_memory_info();
        DiskInfo disk    = read_disk_info();
        uint64_t tasks   = get_total_tasks();

        std::string ip          = get_ip_address();
        std::string cpu_model   = get_cpu_model();
        std::string ram_manu    = get_ram_manufacturer();
        std::string disk_model  = get_disk_model();

        std::string os_name     = get_os_name();
        std::string uptime      = get_uptime_string();

        prev_cpu = curr_cpu;
        prev_net = curr_net;

        // ---- REAL PROCESS DATA ----
        // ---- REAL PROCESS DATA ----
        proc_table.update();

        if (current_sort == "cpu")      proc_table.sort_by_cpu(desc);
        else if (current_sort == "mem") proc_table.sort_by_mem(desc);
        else if (current_sort == "user") proc_table.sort_by_user(desc);
        table.set_data(proc_table.list());

        // ---- DRAW FRAME ----
        screen.begin_frame();
        layout.update();
        layout.draw_borders();

        header.draw_section1(
            os_name,
            uptime,
            cpu_model,
            ram_manu,
            disk_model,
            cpu_total,
            cpu_user,
            cpu_sys,
            cpu_idle,
            mem,
            disk,
            net_mbps,
            tasks,
            ip
        );

        header.draw_process_table_header(current_sort, desc);
        table.draw();


        screen.refresh();

        running = input.handle_input();
    }

    return 0;
}
