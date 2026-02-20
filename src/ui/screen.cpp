#include "ui/screen.hpp"

namespace ui {

Screen::Screen() {
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);
    curs_set(0);

    if (has_colors()) {
        start_color();
        use_default_colors();
        init_colors();
    }

    update_size();
}

Screen::~Screen() {
    endwin();
}

void Screen::init_colors() {
    init_pair(1, COLOR_WHITE, COLOR_BLACK);    // default
    init_pair(2, COLOR_BLACK, COLOR_GREEN);    // highlight row
    init_pair(3, COLOR_CYAN, COLOR_BLACK);     // headers
    init_pair(4, COLOR_YELLOW, COLOR_BLACK);   // info fields
    init_pair(5, COLOR_BLACK, COLOR_BLUE);     // background window effect
}

void Screen::update_size() {
    getmaxyx(stdscr, term_height, term_width);
}

void Screen::draw_background() {
    for (int y = 0; y < term_height; ++y) {
        mvhline(y, 0, ' ', term_width);
    }
}

void Screen::begin_frame() {
    update_size();   // dynamic resizing support
    erase();
    draw_background();  // full coverage background
}

void Screen::refresh() {
    ::refresh();
}

void Screen::end() {
    endwin();
}

int Screen::width() const {
    return term_width;
}

int Screen::height() const {
    return term_height;
}

} // namespace ui
