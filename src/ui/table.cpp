#include <ncurses.h>
#include <algorithm>
#include <iomanip>
#include <sstream>
#include "ui/table.hpp"

namespace ui {

Table::Table(const Layout& lay) : layout(lay) {}

void Table::recalc_visible_rows() {
    auto sec2 = layout.get_sec2();
    visible_rows = sec2.height - 3; // header + separator
    if (visible_rows < 0) visible_rows = 0;
}

void Table::set_data(const std::vector<ProcessInfo>& processes) {
    data = processes;

    // --- Handle sticky PID highlight ---
    if (sticky_pid > 0) {
        auto it = std::find_if(data.begin(), data.end(),
                                [this](const ProcessInfo& p){ return p.pid == sticky_pid; });
        if (it != data.end()) {
            cursor_index = std::distance(data.begin(), it);
        } else {
            // process terminated, move cursor to top
            cursor_index = 0;
            sticky_pid = -1;
        }
    } else if (!highlight_locked) {
        // normal behavior if highlight not locked
        if (cursor_index >= (int)data.size())
            cursor_index = std::max(0, (int)data.size() - 1);
    }

    // Ensure cursor is visible
    ensure_cursor_visible();

    // Reset horizontal scroll each refresh
    cmd_scroll_offset = 0;
}

void Table::move_up() {
    if (cursor_index > 0) {
        cursor_index--;
        if (cursor_index < top_index)
            top_index--;
        cmd_scroll_offset = 0;

        // unlock sticky/highlight since user moved manually
        sticky_pid = -1;
        highlight_locked = false;
    }
}

void Table::move_down() {
    if (cursor_index + 1 < (int)data.size()) {
        cursor_index++;
        if (cursor_index >= top_index + visible_rows)
            top_index++;
        cmd_scroll_offset = 0;

        // unlock sticky/highlight since user moved manually
        sticky_pid = -1;
        highlight_locked = false;
    }
}

void Table::lock_cursor(int idx) {
    if (idx >= 0 && idx < (int)data.size()) {
        cursor_index = idx;
        highlight_locked = true;
        sticky_pid = data[idx].pid;   // now sticky on PID

        ensure_cursor_visible();
    }
}

void Table::unlock_cursor() {
    highlight_locked = false;
    sticky_pid = -1;   // clear sticky PID
}

void Table::move_to_top() {
    cursor_index = 0;
    top_index = 0;
    highlight_locked = false;
    sticky_pid = -1;   // clear sticky PID
}

void Table::draw() {
    recalc_visible_rows();

    auto sec2 = layout.get_sec2();
    int start_y = sec2.y + 2;

    int end_index = std::min(top_index + visible_rows, (int)data.size());

    // Clear table area first
    for (int i = 0; i < visible_rows; ++i) {
        mvhline(start_y + i, sec2.x + 1, ' ', sec2.width - 2);
    }

    for (int i = top_index; i < end_index; ++i) {
        bool highlight = (i == cursor_index);
        draw_row(start_y + (i - top_index), data[i], highlight);
    }
}

void Table::draw_row(int y, const ProcessInfo& proc, bool highlight) const {
    auto sec2 = layout.get_sec2();
    int x = sec2.x + 2;

    if (highlight) attron(A_REVERSE);

    // Column widths (same as header)
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

    // Draw row
    mvprintw(y, col_pid,   "%-*d", w_pid, proc.pid);
    mvprintw(y, col_user,  "%-*s", w_user, proc.user.c_str());
    mvprintw(y, col_cpu,   "%*.1f", w_cpu, proc.cpu_percent); // right-align
    mvprintw(y, col_mem,   "%*.1f", w_mem, proc.mem_percent); // right-align
    mvprintw(y, col_state, "%-*c", w_state, proc.state);
    mvprintw(y, col_time,  "%-*s", w_time, proc.time.c_str());

    // Command field with horizontal scroll
    std::string cmd = proc.command;
    if ((int)cmd.length() > cmd_scroll_offset)
        cmd = cmd.substr(cmd_scroll_offset);
    int max_width = sec2.width - (col_cmd + 2);
    if ((int)cmd.length() > max_width) cmd = cmd.substr(0, max_width);
    mvprintw(y, col_cmd, "%s", cmd.c_str());

    if (highlight) attroff(A_REVERSE);
}

void Table::scroll_command_left() {
    if (cmd_scroll_offset > 0) cmd_scroll_offset--;
}

void Table::scroll_command_right() {
    auto sec2 = layout.get_sec2();
    int col_cmd_start = 68; // approximate start of command column
    int max_width = sec2.width - (col_cmd_start + 2); // visible width
    if (cursor_index < (int)data.size()) {
        const auto& cmd = data[cursor_index].command;
        if ((int)cmd.length() - cmd_scroll_offset > max_width)
            cmd_scroll_offset++;
    }
}

void Table::reset_command_scroll() {
    cmd_scroll_offset = 0;
}

} // namespace ui
