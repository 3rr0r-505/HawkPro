#pragma once
#include <vector>
#include <string>
#include "layout.hpp"
#include "process/process.hpp"

namespace ui {

class Table {
public:
    Table(const Layout& layout);

    void set_data(const std::vector<ProcessInfo>& processes);
    void draw();

    void move_up();
    void move_down();
    void scroll_command_left();
    void scroll_command_right();
    void reset_command_scroll();
    void set_cursor_index(int idx) { cursor_index = idx; }
    void ensure_cursor_visible() {
        if (cursor_index < top_index) top_index = cursor_index;
        else if (cursor_index >= top_index + visible_rows)
            top_index = cursor_index - visible_rows + 1; }
    void lock_cursor(int idx);
    void unlock_cursor();
    void move_to_top(); // move highlighter to top (used after sorting)
    void set_sticky_pid(int pid) { sticky_pid = pid; }   // called by InputHandler after search
    void clear_sticky_pid() { sticky_pid = -1; }         // clear when user moves manually or sorts

private:
    const Layout& layout;

    std::vector<ProcessInfo> data;

    int top_index{0};      // first visible row
    int cursor_index{0};   // selected row (global index)
    int visible_rows{0};   // recalculated each frame
    int sticky_pid{-1}; // PID to stick highlight to

    void recalc_visible_rows();
    void draw_row(int y, const ProcessInfo& proc, bool highlight) const;

    int cmd_scroll_offset{0};   // horizontal scroll offset for command column
    int col_cmd_start{0};       // X coordinate of command column (updated in draw_row)
    bool highlight_locked{false};
};
} // namespace ui

