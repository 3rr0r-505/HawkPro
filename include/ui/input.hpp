#pragma once
#include <string>
#include "table.hpp"
#include "process/process.hpp"

namespace ui {

class InputHandler {
public:
    InputHandler(Table& table, ProcessTable& proc_tbl, std::string& sort_col, bool& ascending);

    bool handle_input();

private:
    Table& table;
    ProcessTable& proc_table;  // add this

    std::string& sort_column;
    bool& ascending_order;
};

} // namespace ui