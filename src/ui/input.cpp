#include <ncurses.h>
#include <vector>
#include <string>
#include "ui/input.hpp"
#include "process/process_search.hpp"

namespace ui {

InputHandler::InputHandler(Table& tbl, ProcessTable& proc_tbl, std::string& sort_col, bool& asc)
    : table(tbl), proc_table(proc_tbl), sort_column(sort_col), ascending_order(asc)
{
    // Ensure non-blocking input and enable arrow keys
    nodelay(stdscr, TRUE);
    keypad(stdscr, TRUE);
}

bool InputHandler::handle_input() {
    int ch = getch();  // Non-blocking input
    if (ch == ERR) return true;  // No key pressed

    // --- Flush any extra keys in buffer ---
    int tmp;
    while ((tmp = getch()) != ERR) { /* discard extra keys */ }

    switch (ch) {
        case 6: // Ctrl+F for search
        {
            nodelay(stdscr, FALSE); // allow blocking input
            echo();
            curs_set(1);

            std::vector<char> buf(256, 0);
            mvprintw(LINES - 1, 0, "Search: ");
            getnstr(buf.data(), buf.size() - 1);

            noecho();
            curs_set(0);
            nodelay(stdscr, TRUE); // restore non-blocking

            std::string query(buf.data());
            if (!query.empty()) {
                ProcessSearch search;
                search.set_process_list(proc_table.list());
                int idx = search.search(query);
                if (idx >= 0) {
                    table.set_cursor_index(idx);                               // move highlight to found row
                    table.set_sticky_pid(proc_table.list()[idx].pid);         // stick highlight to PID
                }
            }

            // Clear search bar
            move(LINES - 1, 0);
            clrtoeol();
        }
        break;

        case 'q':
        case 'Q':
            return false;

        case KEY_UP:
            table.move_up();   // automatically clears sticky PID
            break;

        case KEY_DOWN:
            table.move_down(); // automatically clears sticky PID
            break;

        case KEY_PPAGE: // Page Up
            for (int i = 0; i < 5; ++i) table.move_up();
            break;

        case KEY_NPAGE: // Page Down
            for (int i = 0; i < 5; ++i) table.move_down();
            break;

        case KEY_LEFT:
            table.scroll_command_left();
            break;

        case KEY_RIGHT:
            table.scroll_command_right();
            break;

        case 'c':
        case 'C':
            ascending_order = (sort_column == "cpu") ? !ascending_order : true;
            sort_column = "cpu";
            table.reset_command_scroll();
            table.clear_sticky_pid(); // clear sticky PID after sort
            table.move_to_top();      // move highlight to top
            break;

        case 'm':
        case 'M':
            ascending_order = (sort_column == "mem") ? !ascending_order : true;
            sort_column = "mem";
            table.reset_command_scroll();
            table.clear_sticky_pid();
            table.move_to_top();
            break;

        case 'u':
        case 'U':
            ascending_order = (sort_column == "user") ? !ascending_order : true;
            sort_column = "user";
            table.reset_command_scroll();
            table.clear_sticky_pid();
            table.move_to_top();
            break;

        case 't':
        case 'T':
            table.reset_command_scroll();
            table.clear_sticky_pid();
            table.move_to_top();
            break;

        case KEY_RESIZE:
            table.reset_command_scroll();
            break;

        default:
            break;
    }

    return true;
}

} // namespace ui