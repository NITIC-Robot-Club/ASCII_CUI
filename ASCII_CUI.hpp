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
    Label(const std::string& text) : text_(text) {}
    Label(const std::string& text, void (*func)()) : text_(text), func_(func) {}
    void setText(const std::string& text) { text_ = text; }
    void setColor(Color color) { color_ = color; }
    void setBGColor(Color bgcolor) { bgcolor_ = bgcolor; }
    void setStyle(Style style) { style_ = style; }
    void setSelectedColor(Color color) { selected_color_ = color; }
    void setSelectedBGColor(Color bgcolor) { selected_bgcolor_ = bgcolor; }
    void setSelectedStyle(Style style) { selected_style_ = style; }
    void print() { std::cout << "\033[" << (int)style_ << ";" << (int)color_+30 << ";" << (int)bgcolor_+40 << "m" << text_ << "\033[0m"; }
    void printSelected() { std::cout << "\033[" << (int)selected_style_ << ";" << (int)selected_color_+30 << ";" << (int)selected_bgcolor_+40 << "m" << text_ << "\033[0m"; }   
    void setNext(Layout* next) { next_ = next; }
    Layout* next() { return next_; }
    void setFunc(void (*func)()) { func_ = func; }
    void func() { if(func_!=nullptr){func_();} }

private:
    std::string text_;
    Color color_ = Color::Normal;
    Color bgcolor_ = Color::Normal;
    Style style_ = Style::Reset;
    Color selected_color_ = Color::Normal;
    Color selected_bgcolor_ = Color::Normal;
    Style selected_style_ = Style::Reset;
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
                std::cout << ">";
                current_layout_->at(i)->printSelected();
            } else {
                std::cout << " ";
                current_layout_->at(i)->print();
            }
            std::cout << std::endl;
        }
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
private:
    Layout* current_layout_;
    int selected_index_;

};

} // namespace ASCII_CUI

#endif // __ASCII_CUI_HPP__