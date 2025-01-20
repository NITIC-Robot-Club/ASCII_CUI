#include "ASCII_CUI.hpp"

bool EMG_RQ=0, OVA_EMG_EN=0, UVA_EMG_EN=0, OIA_EMG_EN=0;
float V_LIMIT_HIGH=4.2;

using namespace ASCII_CUI;
Layout main_layout = {
    {"駆動電源基板設定", "-> drive_power"},
    {"制御電源基板設定", "-> control_power"},
    {"ロボマス制御基板設定", "-> robomas"}
};
Layout drive_power_layout = {
    {"戻る", "-> main"},
    {"EX_EMG_TRG","自動非常停止設定"},
    {"EMG_RQ","非常停止要求", vSet(&EMG_RQ)},
    {"V_LIMIT_HIGH","セル当たりの最大電圧アラート", vSet(&V_LIMIT_HIGH)}
};
Layout EX_EMG_TRG_layout = {
    {"戻る", "-> drive_power"},
    {"OVA_EMG_EN", "過電圧アラート時", vSet(&OVA_EMG_EN)},
    {"UVA_EMG_EN", "低電圧アラート時", vSet(&UVA_EMG_EN)},
    {"OIA_EMG_EN", "過電流アラート時", vSet(&OIA_EMG_EN)}
};


UI ui(&main_layout);


int main() {
    main_layout.at(0)->setNext(&drive_power_layout);
    drive_power_layout.at(0)->setNext(&main_layout);
    drive_power_layout.at(1)->setNext(&EX_EMG_TRG_layout);
    EX_EMG_TRG_layout.at(0)->setNext(&drive_power_layout);
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
