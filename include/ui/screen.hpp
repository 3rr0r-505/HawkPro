#pragma once
#include <ncurses.h>

namespace ui {

class Screen {
public:
    Screen();
    ~Screen();

    void begin_frame();     // prepare frame (resize + background)
    void refresh();         
    void end();

    int width() const;
    int height() const;

private:
    int term_width{};
    int term_height{};

    void update_size();     // dynamic resize handling
    void init_colors();
    void draw_background(); // full terminal background
};

} // namespace ui
