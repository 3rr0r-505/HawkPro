#include <ncurses.h>
#include "ui/layout.hpp"

namespace ui {

Layout::Layout(Screen& scr) : screen(scr) {
    calculate_sections();
}

void Layout::update() {
    calculate_sections();   // call every frame before drawing
}

void Layout::calculate_sections() {
    int w = screen.width();
    int h = screen.height();

    // ===== Section 1 (Fixed Height for Stable UI) =====
    int sec1_height = 7;   // matches your static design layout

    if (h < 10) {
        sec1_height = h / 2;   // safety for very small terminals
    }

    sec1.x = 0;
    sec1.y = 0;
    sec1.width = w;
    sec1.height = sec1_height;

    // ===== Section 2 (Remaining Space) =====
    sec2.x = 0;
    sec2.y = sec1_height;
    sec2.width = w;
    sec2.height = h - sec1_height;
}

Section Layout::get_sec1() const {
    return sec1;
}

Section Layout::get_sec2() const {
    return sec2;
}

void Layout::draw_borders() const {
    int w = screen.width();
    int h = screen.height();

    // Outer border
    box(stdscr, 0, 0);

    // Divider between sec1 and sec2
    mvhline(sec1.height, 1, ACS_HLINE, w - 2);

    // Left & Right border are handled by box()
    // Bottom border also handled by box()
}

} // namespace ui
