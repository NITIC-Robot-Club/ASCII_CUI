#ifndef __ASCII_CUI_HPP__
#define __ASCII_CUI_HPP__

#include <iostream>
#include <string>
#include <vector>

namespace ASCII_CUI {

enum class Color {
    Black   = 0,
    Red     = 1,
    Green   = 2,
    Yellow  = 3,
    Blue    = 4,
    Magenta = 5,
    Cyan    = 6,
    White   = 7,
    Normal  = 9
};

enum class Style {
    Reset       = 0,
    Bold        = 1,
    Underline   = 4,
    Blink       = 5,
    Reverse     = 7,
    Invisible   = 8
};

class Layout;

class Label {
public:
    Label(const std::string& title, const std::string& text) : title_(title), text_(text) {}
    Label(const std::string& title, const std::string& text, void (*func)()) : title_(title), text_(text), func_(func) {}
    void setTitle(const std::string& title) { title_ = title; }
    void setText(const std::string& text) { text_ = text; }
    void setColor(Color color) { color_ = color; }
    void setBGColor(Color bgcolor) { bgcolor_ = bgcolor; }
    void setStyle(Style style) { style_ = style; }
    void setSelectedColor(Color color) { selected_color_ = color; }
    void setSelectedBGColor(Color bgcolor) { selected_bgcolor_ = bgcolor; }
    void setSelectedStyle(Style style) { selected_style_ = style; }
    void print() {
        std::cout   << "\033["
                    << (int)style_ << ";"
                    << (int)color_+30 << ";"
                    << (int)bgcolor_+40 << "m"
                    << title_ << "\033[0m";
    }
    void printSelected(int n) {
        std::cout   << "\033["
                    << (int)selected_style_ << ";"
                    << (int)selected_color_+30 << ";"
                    << (int)selected_bgcolor_+40 << "m"
                    << title_ << "\033[0m";
        std::cout   << "\033[" << n << ";0H" << "\033[J" << text_;
    }   
    void setNext(Layout* next) { next_ = next; }
    Layout* next() { return next_; }
    void setFunc(void (*func)()) { func_ = func; }
    void func() { if(func_!=nullptr){func_();} }

private:
    std::string title_;
    std::string text_;
    Color color_ = Color::Normal;
    Color bgcolor_ = Color::Normal;
    Style style_ = Style::Reset;
    Color selected_color_ = Color::Normal;
    Color selected_bgcolor_ = Color::Normal;
    Style selected_style_ = Style::Underline;
    Layout* next_ = nullptr;
    void (*func_)() = nullptr;
};

class Layout {
public:
    Layout(std::initializer_list<Label> labels) : labels_(labels) {}
    Label* at(int index) { return &labels_[index]; }
    int size() { return labels_.size(); }
private:
    std::vector<Label> labels_;
};

class UI {
public:
    UI(Layout* layout) : current_layout_(layout), selected_index_(0) { std::cout << "\033[2J"; }
    void print() {
        // move to 0,0
        std::cout << "\033[0;0H";
        for (int i = 0; i < current_layout_->size(); i++) {
            if (i == selected_index_) {
                // move to i,0
                std::cout << "\033[" << i+1 << ";1H"; 
                std::cout << " > ";
                current_layout_->at(i)->printSelected(current_layout_->size()+2);
            } else {
                std::cout << "\033[" << i+1 << ";1H"; 
                std::cout << "   ";
                current_layout_->at(i)->print();
            }
            std::cout << std::endl;
        }
        std::cout << "\033[" << current_layout_->size()+1 << ";0H";
        std::cout << "-------------------------------------------" << std::endl << std::endl;
    }

    void up() {
        if (selected_index_ > 0) {
            selected_index_--;
        }
    }

    void down() {
        if (selected_index_ < current_layout_->size() - 1) {
            selected_index_++;
        }
    }

    void enter() {
        current_layout_->at(selected_index_)->func();
        auto next = current_layout_->at(selected_index_)->next();
        if (next != nullptr) {
            current_layout_ = next;
            selected_index_ = 0;
        }
        std::cout << "\033[2J\033[0;0H";
    }

    template <typename T>
    void popup(const std::string& message, T* input) {
        std::cout << "\033[2J";
        std::cout << message << std::endl;
        std::cout << ">>> ";
        std::cin >> *input;
        std::cout << "\033[2J";
    }

private:
    Layout* current_layout_;
    int selected_index_;

};

} // namespace ASCII_CUI

#endif // __ASCII_CUI_HPP__