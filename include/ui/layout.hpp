#pragma once
#include "screen.hpp"

namespace ui {

struct Section {
    int x;
    int y;
    int width;
    int height;
};

class Layout {
public:
    Layout(Screen& scr);

    void update();                 // recalc every frame (resize safe)

    Section get_sec1() const;
    Section get_sec2() const;

    void draw_borders() const;     // clean box style borders

private:
    Screen& screen;
    Section sec1{};
    Section sec2{};

    void calculate_sections();
};

} // namespace ui
