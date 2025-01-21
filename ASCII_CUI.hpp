#ifndef __ASCII_CUI_HPP__
#define __ASCII_CUI_HPP__

#include <iostream>
#include <string>
#include <vector>
#include <typeindex>
#include <unordered_map>

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

std::unordered_map<std::type_index, int> type_map = {
    {std::type_index(typeid(uint8_t) ), 0 },
    {std::type_index(typeid(uint16_t)), 1 },
    {std::type_index(typeid(int32_t) ), 2 },
    {std::type_index(typeid(uint64_t)), 3 },
    {std::type_index(typeid(float)   ), 4 },
    {std::type_index(typeid(bool)    ), 5 },
    {std::type_index(typeid(double)  ), 6 },
    {std::type_index(typeid(void)    ), -1}
};

class Variable {
public:
    int type;
    void* address[7] = {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr};

    friend std::ostream& operator<<(std::ostream& os, const Variable& v) {
        if(v.address[v.type] == nullptr) {
            return os;
        }
        switch(v.type) {
            case 0: os << *(uint8_t*) v.address[v.type]; break;
            case 1: os << *(uint16_t*)v.address[v.type]; break;
            case 2: os << *(int32_t*) v.address[v.type]; break;
            case 3: os << *(uint64_t*)v.address[v.type]; break;
            case 4: os << *(float*)   v.address[v.type]; break;
            case 5: os << *(bool*)    v.address[v.type]; break;
            case 6: os << *(double*)  v.address[v.type]; break;
            default: break;
        }
        return os;
    }

    friend std::istream& operator>>(std::istream& is, Variable& v) {
        if(v.type == -1) {
            return is;
        }
        switch(v.type) {
            case 0: is >> *(uint8_t*) v.address[v.type]; break;
            case 1: is >> *(uint16_t*)v.address[v.type]; break;
            case 2: is >> *(int32_t*) v.address[v.type]; break;
            case 3: is >> *(uint64_t*)v.address[v.type]; break;
            case 4: is >> *(float*)   v.address[v.type]; break;
            case 5: is >> *(bool*)    v.address[v.type]; break;
            case 6: is >> *(double*)  v.address[v.type]; break;
            default: break;
        }
        return is;
    }
};

std::string get_typename(int type) {
    switch(type) {
        case 0: return "uint8_t";
        case 1: return "uint16_t";
        case 2: return "int32_t";
        case 3: return "uint64_t";
        case 4: return "float";
        case 5: return "bool";
        case 6: return "double";
        default: return "void";
    }
}

template <typename T>
Variable vSet(T* variable) {
    Variable v;
    v.type = type_map[std::type_index(typeid(T))];
    v.address[v.type] = variable;
    return v;
}

Variable vSet(std::nullptr_t) {
    Variable v;
    v.type = type_map[std::type_index(typeid(void))];
    return v;
}

class Layout;

class Label {
public:
    Label(const std::string& title, const std::string& text, Variable variable = vSet(nullptr))
        : title_(title), text_(text), variable_(variable) {}
    void setTitle(const std::string& title) { title_ = title; }
    void setText(const std::string& text) { text_ = text; }
    void setColor(Color color) { color_ = color; }
    void setBGColor(Color bgcolor) { bgcolor_ = bgcolor; }
    void setStyle(Style style) { style_ = style; }
    void setSelectedColor(Color color) { selected_color_ = color; }
    void setSelectedBGColor(Color bgcolor) { selected_bgcolor_ = bgcolor; }
    void setSelectedStyle(Style style) { selected_style_ = style; }
    void setNext(Layout* next) { next_ = next; }
    template <typename T>
    void setVariable(T* variable) {
        variable_ = vSet(variable);
    }
    void select() {
        if(variable_.type == -1) {
            return;
        }
        if(variable_.type == 5) {
            *(bool*)variable_.address[variable_.type] = !*(bool*)variable_.address[variable_.type];
        } else {
            std::cout << "\033[2J";
            std::cout << "\033[2;1H\033[J" << text_ << std::endl;
            std::cout << title_ << "の値を入力してください (" << get_typename(variable_.type) << ")" <<std::endl;
            std::cout << "現在の値 : " << variable_ << std::endl;
            std::cout << ">>> ";
            std::cin >> variable_;
            std::cout << "\033[2J";
        }
    }

    void print() {
        std::cout << "\033[" << (int)style_ << ";" << (int)color_ + 30 << ";" 
                  << (int)bgcolor_ + 40 << "m" << title_ << "\033[0m";
    }

    void printSelected(int n) {
        std::cout << "\033[" << (int)selected_style_ << ";" 
                  << (int)selected_color_ + 30 << ";" 
                  << (int)selected_bgcolor_ + 40 << "m" 
                  << title_ << "\033[0m";
        std::cout << "\033[" << n << ";1H\033[J" << text_;
        if (variable_.type != -1) {
            std::cout << " : " << variable_;
        }
    }
    Layout* next() { return next_; }

private:
    std::string title_;
    std::string text_;
    Variable variable_;
    Layout* next_ = nullptr;
    Color color_ = Color::Normal;
    Color bgcolor_ = Color::Normal;
    Style style_ = Style::Reset;
    Color selected_color_ = Color::Normal;
    Color selected_bgcolor_ = Color::Normal;
    Style selected_style_ = Style::Underline;
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

    template <typename T>
    void popup(const std::string& message, T* input) {
        std::cout << "\033[2J";
        std::cout << message << std::endl;
        std::cout << ">>> ";
        std::cin >> *input;
        std::cout << "\033[2J";
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
        auto next = current_layout_->at(selected_index_)->next();
        if (next != nullptr) {
            current_layout_ = next;
            selected_index_ = 0;
        } else {
            current_layout_->at(selected_index_)->select();
        }
        std::cout << "\033[2J\033[0;0H";
    }

private:
    Layout* current_layout_;
    int selected_index_;

};

} // namespace ASCII_CUI

#endif // __ASCII_CUI_HPP__

