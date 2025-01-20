#ifndef __ASCII_CUI_HPP__
#define __ASCII_CUI_HPP__

#include <iostream>
#include <string>
#include <vector>
#include <stdarg.h>

namespace ASCII_CUI {
    struct Color {
        struct FG {
            static constexpr const char* Black = "\033[30m";
            static constexpr const char* Red = "\033[31m";
            static constexpr const char* Green = "\033[32m";
            static constexpr const char* Yellow = "\033[33m";
            static constexpr const char* Blue = "\033[34m";
            static constexpr const char* Magenta = "\033[35m";
            static constexpr const char* Cyan = "\033[36m";
            static constexpr const char* White = "\033[37m";
            static constexpr const char* Normal = "\033[39m";
        };
        struct BG {
            static constexpr const char* Black = "\033[40m";
            static constexpr const char* Red = "\033[41m";
            static constexpr const char* Green = "\033[42m";
            static constexpr const char* Yellow = "\033[43m";
            static constexpr const char* Blue = "\033[44m";
            static constexpr const char* Magenta = "\033[45m";
            static constexpr const char* Cyan = "\033[46m";
            static constexpr const char* White = "\033[47m";
            static constexpr const char* Normal = "\033[49m";
        };
    };

    struct Style {
        static constexpr const char* Reset = "\033[0m";
        static constexpr const char* Bold = "\033[1m";
        static constexpr const char* Underline = "\033[4m";
        static constexpr const char* Inverse = "\033[7m";
    };

    typedef struct Text_type {
        const char* bgcolor;
        const char* color;
        const char* style;
        std::string text;
    } Text;

    class Layout {
    public:
        ASCII_CUI::Text text;
        void (*callback)();
        Layout *next;
        Layout();
        ~Layout();
    };

    typedef struct Layouts_type {
        std::vector<Layout> texts;
    } Layouts;

    class Cursor {
    public:
        int x;
        int y;
        Cursor();
        ~Cursor();
        void move(int x, int y);
        void moveUp(int n);
        void moveDown(int n);
        void moveLeft(int n);
        void moveRight(int n);
    };

    class Screen {
    public:
        Screen();
        ~Screen();
        void clear();
        void print(const char* format, ...);
        void print(const Text& text);
    };

    class UI {
    public:
        // cursor cursor;
        // screen screen;
        // layouts layouts;
        // ui();
        // ~ui();
    };
}

#endif // __ASCII_CUI_HPP__