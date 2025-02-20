# ascii_cui
ASCIIコードのキー入力(矢印、スペース、エンター)でCUI(BIOS的な？)を動かす

# サンプルコード
```cpp
#include "ASCII_CUI.hpp"

// 事前に設定したい変数を定義
bool myBool = false;
float myFloat = 1.0f;
uint8_t  myUint8t  = 1;
uint16_t myUint16t = 2;


// 画面のレイアウトを定義
ASCII_CUI::Layout main_layout, sub_layout, subsub_layout;

// main_layoutを初期状態でUI作成
ASCII_CUI::UI UI(&main_layout);

// 変更時に呼び出される関数を作成
void on_bool_change(void) {
    if (myBool) {
        UI << "myBool is true" << std::endl;
    } else {
        UI << "myBool is false" << std::endl;
    }
}

void setup() {
    main_layout = ASCII_CUI::Layout({
        {"myBool", "BOOLのテスト", ASCII_CUI::Variable(&myBool), &on_bool_change},
        {"sub設定", "-> sub", &sub_layout}},
        "これはメイン画面表示中のタイトル"
    );

    sub_layout = ASCII_CUI::Layout({
        {"戻る", "-> main", &main_layout},    // mainに戻る用を追加
        {"myFloat", "FLOATのテスト", ASCII_CUI::Variable(&myFloat)},
        {"subsub設定", "-> subsub", &subsub_layout}},
        "main -> sub"
    );

    subsub_layout = ASCII_CUI::Layout({
        {"戻る", "-> sub", &sub_layout},    // subに戻る用を追加
        {"myUint8t", "UINT8_Tのテスト", ASCII_CUI::Variable(&myUint8t)},
        {"myUint16t", "UINT16_Tのテスト", ASCII_CUI::Variable(&myUint16t)}},
        "main -> sub -> subsub"
    );

}

int main() {
    setup();
    while (true) {
        UI.print(); // 画面更新
        int c = getchar();
        if (c == 'w') {
            UI.up();    // 上
        } else if (c == 's') {
            UI.down();  // 下
        } else if (c == 'e') {
            UI.enter(); // 確定
        }
    }
    return 0;
}

```

# 仕様

## ASCII_CUI::Color

| 値       | 説明       |
|----------|------------|
| Black    | 黒         |
| Red      | 赤         |
| Green    | 緑         |
| Yellow   | 黄         |
| Blue     | 青         |
| Magenta  | マゼンタ   |
| Cyan     | シアン     |
| White    | 白         |
| Normal   | 通常       |

- Labelに対して設定します
- 文字色、背景色に設定できます
- (WIP) RGB指定

## ASCII_CUI::Style

| 値       | 説明       |
|----------|------------|
| Reset    | リセット   |
| Bold     | 太字       |
| Underline| 下線       |
| Blink    | 点滅       |
| Reverse  | 反転       |
| Invisible| 不可視     |

- Labelに対して設定します

## ASCII_CUI::Label

| メンバ変数      | 型                       | 説明                           |
|-----------------|--------------------------|--------------------------------|
| title           | std::string              | ラベルのタイトル               |
| text            | std::string              | ラベルのテキスト               |
| next            | ASCII_CUI::Layout*       | 次のレイアウトへのポインタ     |
| color           | ASCII_CUI::Color         | ラベルの色                     |
| bgcolor         | ASCII_CUI::Color         | ラベルの背景色                 |
| style           | ASCII_CUI::Style         | ラベルのスタイル               |
| selected_color  | ASCII_CUI::Color         | 選択時のラベルの色             |
| selected_bgcolor| ASCII_CUI::Color         | 選択時のラベルの背景色         |
| selected_style  | ASCII_CUI::Style         | 選択時のラベルのスタイル       |
| variable        | ASCII_CUI::Variable      | ラベルに関連付けられた変数     |
| callback        | void (*)(void)           | ラベルが決定されたときのコールバック関数 |

| メンバ関数　    | 説明                           |
|-----------------|--------------------------------|
| select()        | ラベルを選択したときの処理     |
| print()         | ラベルを表示                   |
| printSelected() | 選択されたラベルを表示         |

- ASCII_CUI::Color,Styleを指定したいときは `Label.color = ASCII_CUI::Color::Normal;`のように書きます
- variableを設定するときは、`ASCII_CUI::vSet(&変数)`でASCII_CUI::Variableに変換して渡します
- callback関数は変数の変更後に呼び出されます
- callbackはvoid関数のアドレスを指定してください
- メンバ関数は内部で使用するのでユーザーは無視してください(おい！)

## ASCII_CUI::Layout

| メンバ関数      | 説明                           |
|-----------------|--------------------------------|
| operator[]()    | インデックスでラベルにアクセス |
| size()          | レイアウトのサイズを取得       |
| at()            | インデックスでラベルにアクセス |

- LayoutはLabelのvectorのようなものです
- [], atでLabelにアクセスできます

## ASCII_CUI::UI

| メンバ変数      | 型                       | 説明                           |
|-----------------|--------------------------|--------------------------------|
| debug_log_length| int                      | デバッグログの長さ             |
| debug_log_place | int                      | デバッグログの表示位置         |

| メンバ関数      | 説明                           |
|-----------------|--------------------------------|
| print()         | レイアウトを表示               |
| popup()         | ポップアップを表示             |
| up()            | 選択を上に移動                 |
| down()          | 選択を下に移動                 |
| enter()         | 選択を確定                     |
| operator<<()    | デバッグログを出力             |

- Layoutを表示するためのものです
- 同時に仕様することは想定していません
- `UI << "Hello World!" << std::endl;`でデバッグログを出力できます
- 使用するインターフェイス(マイコンのUARTやROS2 keyboard topicなど)に応じてup,down,enterを実行してください


# サンプル2
```cpp
#include "ASCII_CUI.hpp"

// 事前に設定したい変数を定義
bool EMG_RQ=0, OVA_EMG_EN=0, UVA_EMG_EN=0, OIA_EMG_EN=0;
float V_LIMIT_HIGH=4.2f;

// 画面のレイアウトを定義
ASCII_CUI::Layout main_layout, drive_power_layout, EX_EMG_TRG_layout;

// main_layoutを初期状態でUI作成
ASCII_CUI::UI UI(&main_layout);

// 変更時に呼び出される関数を作成
void on_emg_change(void) {
    if (EMG_RQ) {
        UI << "EMG_RQがONになりました" << std::endl;
    } else {
        UI << "EMG_RQがOFFになりました" << std::endl;
    }
}

void setup() {
    main_layout = {
        {"駆動電源基板設定", "-> drive_power", &drive_power_layout},
        {"制御電源基板設定", "-> control_power"},
        {"ロボマス制御基板設定", "-> robomas"}
    };

    drive_power_layout = ASCII_CUI::Layout({
        {"戻る", "-> main", &main_layout},    // mainに戻る用を追加
        {"EX_EMG_TRG","自動非常停止設定", &EX_EMG_TRG_layout},
        {"EMG_RQ","非常停止要求", ASCII_CUI::Variable(&EMG_RQ), &on_emg_change},    // 関数の登録
        {"V_LIMIT_HIGH","セル当たりの最大電圧アラート", ASCII_CUI::Variable(&V_LIMIT_HIGH)}},
        "main -> drive_power"
    );

    EX_EMG_TRG_layout = ASCII_CUI::Layout({
        {"戻る", "-> drive_power", &drive_power_layout}, // 戻る用
        {"OVA_EMG_EN", "過電圧アラート時", ASCII_CUI::Variable(&OVA_EMG_EN)},
        {"UVA_EMG_EN", "低電圧アラート時", ASCII_CUI::Variable(&UVA_EMG_EN)},
        {"OIA_EMG_EN", "過電流アラート時", ASCII_CUI::Variable(&OIA_EMG_EN)}},
        "main -> drive_power -> EX_EMG_TRG"
    );
}

int main() {
    setup();
    while (true) {
        UI.print(); // 画面更新
        int c = getchar();
        if (c == 'w') {
            UI.up();    // 上
        } else if (c == 's') {
            UI.down();  // 下
        } else if (c == 'e') {
            UI.enter(); // 確定
        }
    }
    return 0;
}
```
