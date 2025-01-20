#include "ASCII_CUI.hpp"

namespace ASCII_CUI {
    Cursor::Cursor() {
        move(0, 0);
    }

    Cursor::~Cursor() {
    }

    void Cursor::move(int x, int y) {
        this->x = x;
        this->y = y;
        std::cout << "\033[" << y << ";" << x << "H";
    }

    void Cursor::moveUp(int n) {
        y -= n;
        std::cout << "\033[" << n << "A";
    }

    void Cursor::moveDown(int n) {
        y += n;
        std::cout << "\033[" << n << "B";
    }

    void Cursor::moveLeft(int n) {
        x -= n;
        std::cout << "\033[" << n << "D";
    }

    void Cursor::moveRight(int n) {
        x += n;
        std::cout << "\033[" << n << "C";
    }

    Screen::Screen() {
        clear();
    }

    void Screen::clear() {
        std::cout << "\033[2J";
    }

    void Screen::print(const char* format, ...) {
        va_list args;
        va_start(args, format);
        vprintf(format, args);
        va_end(args);
    }

    void Screen::print(const Text& text) {
        std::cout << text.color << text.bgcolor << text.style << text.text;
        std::cout << Color::FG::Normal << Color::BG::Normal << Style::Reset;
    }


    Screen::~Screen() {
    }
};