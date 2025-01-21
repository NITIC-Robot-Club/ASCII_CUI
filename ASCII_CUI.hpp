#ifndef __ASCII_CUI_HPP__
#define __ASCII_CUI_HPP__

#include <iostream>
#include <string>
#include <vector>
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

template<typename T> struct typeIndex;
template<> struct typeIndex<uint8_t>  { static const int index = 0;  };
template<> struct typeIndex<uint16_t> { static const int index = 1;  };
template<> struct typeIndex<int32_t>  { static const int index = 2;  };
template<> struct typeIndex<uint64_t> { static const int index = 3;  };
template<> struct typeIndex<float>    { static const int index = 4;  };
template<> struct typeIndex<bool>     { static const int index = 5;  };
template<> struct typeIndex<double>   { static const int index = 6;  };
template<> struct typeIndex<void>     { static const int index = -1; };

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


template <typename T>
Variable vSet(T* variable) {
    Variable v;
    v.type = typeIndex<T>::index;
    v.address[v.type] = variable;
    return v;
}

Variable vSet(std::nullptr_t) {
    Variable v;
    v.type = typeIndex<void>::index;
    return v;
}

class Layout;

class Label {
public:
    Label() {}
    Label(const std::string& title, const std::string& text)
        : title(title), text(text) {}
    Label(const std::string& title, const std::string& text, Layout* next)
        : title(title), text(text), next(next) {}
    Label(const std::string& title, const std::string& text, void (*callback)(void))
        : title(title), text(text), callback(callback) {}
    Label(const std::string& title, const std::string& text, Variable variable)
        : title(title), text(text), variable(variable) {}
    Label(const std::string& title, const std::string& text, Variable variable, void (*callback)(void))
        : title(title), text(text), variable(variable), callback(callback) {}
    Label(const std::string& title, const std::string& text, void (*callback)(void), Variable variable)
        : title(title), text(text), variable(variable), callback(callback) {}

    std::string title = "";
    std::string text = "";
    Layout* next = nullptr;
    Color color = Color::Normal;
    Color bgcolor = Color::Normal;
    Style style = Style::Reset;
    Color selected_color = Color::Normal;
    Color selected_bgcolor = Color::Normal;
    Style selected_style = Style::Underline;
    Variable variable = vSet(nullptr);
    void (*callback)(void) = nullptr;

    void operator= (const Label& l) {
        title = l.title;
        text = l.text;
        next = l.next;
        color = l.color;
        bgcolor = l.bgcolor;
        style = l.style;
        selected_color = l.selected_color;
        selected_bgcolor = l.selected_bgcolor;
        selected_style = l.selected_style;
        variable = l.variable;
        callback = l.callback;
    }


    void select() {
        if(variable.type == -1) {
            return;
        }
        if(variable.type == 5) {
            *(bool*)variable.address[variable.type] = !*(bool*)variable.address[variable.type];
        } else {
            std::cout << "\033[2J";
            std::cout << "\033[2;1H\033[J" << text << std::endl;
            std::cout << title << "の値を入力してください (" << get_typename(variable.type) << ")" <<std::endl;
            std::cout << "現在の値 : " << variable << std::endl;
            std::cout << ">>> ";
            std::cin >> variable;
            std::cout << "\033[2J";
        }
        if(callback != nullptr) {
            callback();
        }
    }

    void print() {
        std::cout << "\033[" << (int)style << ";" << (int)color + 30 << ";" 
                  << (int)bgcolor + 40 << "m" << title << "\033[0m";
    }

    void printSelected(int n) {
        std::cout << "\033[" << (int)selected_style << ";" 
                  << (int)selected_color + 30 << ";" 
                  << (int)selected_bgcolor + 40 << "m" 
                  << title << "\033[0m";
        std::cout << "\033[" << n << ";1H\033[J" << text;
        if (variable.type != -1) {
            std::cout << " : " << variable;
        }
    }
};


class Layout {
public:
    Layout() {}
    Layout(std::initializer_list<Label> labels) : labels_(labels) {}

    void operator= (const Layout& l) {
        labels_ = l.labels_;
    }

    void operator= (std::initializer_list<Label> labels) {
        labels_ = labels;
    }

    Label& operator[](size_t index) {
        return labels_.at(index);
    }

    size_t size() const {
        return labels_.size();
    }

    Label* at(size_t index) {
        if (index < labels_.size()) {
            return &labels_.at(index);
        }
        return nullptr;
    }

private:
    std::vector<Label> labels_;
};


class UI {
public:
    int debug_log_length = 10;
    int debug_log_place = 50;
    
    UI(Layout* layout) : current_layout_(layout), selected_index_(0) {
        std::cout << "\033[2J";
        debug_log_.resize(debug_log_length);
    }

    void print() {
        std::cout << "\033[0;0H";
        for (int i = 0; i < current_layout_->size(); i++) {
            if (i == selected_index_) {
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
        for (int i=0; i<debug_log_length; i++) {
            std::cout   << "\033["<<i+1<<";"<<debug_log_place<<"H"
                        << "| " <<debug_log_[debug_log_length-i-1] << std::endl;
        }
        std::cout << "\033[" << current_layout_->size()+1 << ";0H";
        std::cout << "-------------------------------------------\n";
        std::cout << "\033[" << std::max((int)current_layout_->size(), debug_log_length) << ";0H";
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
        auto next = current_layout_->at(selected_index_)->next;
        if (next != nullptr) {
            current_layout_ = next;
            selected_index_ = 0;
        } else {
            current_layout_->at(selected_index_)->select();
        }
        std::cout << "\033[2J\033[0;0H";
    }
    UI& operator<<(const std::string& log) {
        if(debug_log_length != debug_log_.size()) {
            debug_log_.resize(debug_log_length);
        }
        for (int i = debug_log_length-1; i > 0; i--) {
            debug_log_[i] = debug_log_[i-1];
        }
        debug_log_[0] = log;
        return *this;
    }
    
    UI& operator<<(std::ostream& (*os)(std::ostream&)) {
        print();
        return *this;
    }


private:
    Layout* current_layout_;
    int selected_index_;
    std::vector<std::string> debug_log_;

};

} // namespace ASCII_CUI

#endif // __ASCII_CUI_HPP__

