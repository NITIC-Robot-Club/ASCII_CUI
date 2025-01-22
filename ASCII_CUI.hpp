#ifndef __ASCII_CUI_HPP__
#define __ASCII_CUI_HPP__

#include <iostream>
#include <string>
#include <cstdint>
#include <vector>
#include <unordered_map>

namespace ASCII_CUI {

enum class Color {
    Black   = 0,    // 黒
    Red     = 1,    // 赤
    Green   = 2,    // 緑
    Yellow  = 3,    // 黄
    Blue    = 4,    // 青
    Magenta = 5,    // 紫
    Cyan    = 6,    // 水
    White   = 7,    // 白
    Normal  = 9     // デフォルト
};

enum class Style {
    Reset       = 0,    // リセット
    Bold        = 1,    // 太字
    Underline   = 4,    // 下線
    Blink       = 5,    // 点滅
    Reverse     = 7,    // 反転
    Invisible   = 8     // 不可視
};

enum class Key {
    Up      = 65,   // ↑
    Down    = 66,   // ↓
    Right   = 67,   // →
    Left    = 68,   // ←
    Enter   = 10,   // Enter
    Space   = 32,   // Space
    Esc     = 27    // Esc
};


inline std::string setPosition(size_t x, size_t y, bool is_title=false) {
    if(is_title) {
        return "\033[" + std::to_string(x) + ";" + std::to_string(y) + "H";
    }
    return "\033[" + std::to_string(x+1) + ";" + std::to_string(y) + "H";
}

inline std::string setColor(Color color) {
    return "\033[" + std::to_string((int)color + 30) + "m";
}

inline std::string setBGColor(Color color) {
    return "\033[" + std::to_string((int)color + 40) + "m";
}

inline std::string setStyle(Style style) {
    return "\033[" + std::to_string((int)style) + "m";
}

const std::string clearScreen = "\033[2J";
const std::string clearLine = "\033[2K";
const std::string resetStyle = "\033[0m";


template<typename T> struct typeIndex;
template<> struct typeIndex<uint8_t>  { static const int index = 0;  };
template<> struct typeIndex<uint16_t> { static const int index = 1;  };
template<> struct typeIndex<int32_t>  { static const int index = 2;  };
template<> struct typeIndex<uint64_t> { static const int index = 3;  };
template<> struct typeIndex<float>    { static const int index = 4;  };
template<> struct typeIndex<bool>     { static const int index = 5;  };
template<> struct typeIndex<double>   { static const int index = 6;  };
// !追加の型記述 1
// void型の場合は-1を返す
template<> struct typeIndex<void>     { static const int index = -1; };

inline std::string get_typename(int type) {
    switch(type) {
        case 0: return "uint8_t";
        case 1: return "uint16_t";
        case 2: return "int32_t";
        case 3: return "uint64_t";
        case 4: return "float";
        case 5: return "bool";
        case 6: return "double";
        // !追加の型記述 2
        // -1はvoid型, それ以外はエラー
        case -1: return "void";
        default: return "error";
    }
}

class Variable {
public:
    int type;
    // 型を増やすときは長さを変更
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
            // !追加の型記述 3
            // void型の場合は何もしない
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
            // !追加の型記述 4
            // void型の場合は何もしない
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

inline Variable vSet(std::nullptr_t) {
    Variable v;
    v.type = typeIndex<void>::index;
    return v;
}

class Layout;

class Label {
public:
    Label() {}
    Label(const std::string& title_, const std::string& text_)
        : title(title_), text(text_) {}
    Label(const std::string& title_, const std::string& text_, Layout* next_)
        : title(title_), text(text_), next(next_) {}
    Label(const std::string& title_, const std::string& text_, void (*callback_)(void))
        : title(title_), text(text_), callback(callback_) {}
    Label(const std::string& title_, const std::string& text_, Variable variable_)
        : title(title_), text(text_), variable(variable_) {}
    Label(const std::string& title_, const std::string& text_, Variable variable_, void (*callback_)(void))
        : title(title_), text(text_), variable(variable_), callback(callback_) {}
    Label(const std::string& title_, const std::string& text_, void (*callback_)(void), Variable variable_)
        : title(title_), text(text_), variable(variable_), callback(callback_) {}

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

    void select(std::string title_) {
        if(variable.type == -1) {
            return;
        }
        if(variable.type == 5) {
            *(bool*)variable.address[variable.type] = !*(bool*)variable.address[variable.type];
        } else {
            std::cout << setPosition(0,0) << clearScreen << text << std::endl;
            std::cout << title_ << std::endl;
            std::cout << title << "の値を入力してください (" << get_typename(variable.type) << ")" <<std::endl;
            std::cout << "現在の値 : " << variable << std::endl;
            std::cout << ">>> ";
            std::cin >> variable;
            std::cout << clearScreen;
        }
        if(callback != nullptr) {
            callback();
        }
    }

    void print() {
        std::cout << setColor(color) << setBGColor(bgcolor) << setStyle(style) << title << resetStyle;
    }

    void printSelected(size_t n) {
        std::cout << setColor(selected_color) << setBGColor(selected_bgcolor) << setStyle(selected_style) << title << resetStyle;
        std::cout << setPosition(n,0) << text;
        if (variable.type != -1) {
            std::cout << " : " << variable;
        }
    }
};


class Layout {
public:
    Layout() {}
    Layout(std::initializer_list<Label> labels) : title(""), labels_(labels) {}
    Layout(std::initializer_list<Label> labels, std::string title_) : title(title_), labels_(labels) {}

    std::string title;

    void operator= (const Layout& l) {
        labels_ = l.labels_;
        title = l.title;
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
    size_t debug_log_length = 10;
    size_t debug_log_place = 50;
    
    UI(Layout* layout) : current_layout_(layout), selected_index_(0) {
        std::cout << clearScreen;
        debug_log_.resize(debug_log_length);
    }

    void print() {
        std::cout << setPosition(0,0) << clearScreen;
        if(current_layout_->title != "") {
            std::cout << current_layout_->title << std::endl;
        }
        for (size_t i = 0; i < current_layout_->size(); i++) {
            if (i == selected_index_) {
                std::cout << setPosition(i+1,0) << " > ";
                current_layout_->at(i)->printSelected(current_layout_->size()+3);
            } else {; 
                std::cout << setPosition(i+1,0) << "   ";
                current_layout_->at(i)->print();
            }
            std::cout << std::endl;
        }
        for (size_t i=0; i<debug_log_length; i++) {
            std::cout   << setPosition(i,debug_log_place-1)
                        << "| " <<debug_log_[debug_log_length-i-1] << std::endl;
        }
        std::cout << setPosition(current_layout_->size()+2, 0);
        std::cout << "-------------------------------------------\n";
        std::cout << setPosition(std::max(current_layout_->size(),debug_log_length),0);
    }

    template <typename T>
    void popup(const std::string& message, T* input) {
        std::cout << clearScreen << message << std::endl;
        std::cout << ">>> ";
        std::cin >> *input;
        std::cout << clearScreen;
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
            current_layout_->at(selected_index_)->select(current_layout_->title);
        }
        print();
    }

    UI& operator<<(const std::string& log) {
        debug(log);
        return *this;
    }
    
    UI& operator<<(std::ostream& (*)(std::ostream&)) {
        print();
        return *this;
    }

    void debug(const std::string& log) {
        if(debug_log_length != debug_log_.size()) {
            debug_log_.resize(debug_log_length);
        }
        for (size_t i = debug_log_length-1; i > 0; i--) {
            debug_log_[i] = debug_log_[i-1];
        }
        debug_log_[0] = log;
    }


private:
    Layout* current_layout_;
    size_t selected_index_;
    std::vector<std::string> debug_log_;
};

} // namespace ASCII_CUI

#endif // __ASCII_CUI_HPP__

