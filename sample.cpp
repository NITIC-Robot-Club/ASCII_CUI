#include "ASCII_CUI.hpp"

void func1() {
    std::cout << "func 1" << std::endl;
}


void func11() {
    std::cout << "func 1-1" << std::endl;
}

int main() {
    using namespace ASCII_CUI;
    Layout layout1 = {
        {"hello world (1)", func1},
        {"hello world (2)"}
    };
    Layout layout2 = {
        {"hello world (1-1)", func11}
    };
    layout1.at(0)->setNext(&layout2);
    layout2.at(0)->setNext(&layout1);
    UI ui(&layout1);
    while (true) {
        ui.print();
        char c = getchar();
        if (c == 'w') {
            ui.up();
        } else if (c == 's') {
            ui.down();
        } else if (c == 'e') {
            ui.enter();
        }
    }
    return 0;
}