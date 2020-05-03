//
// Created by lars on 03.05.20.
//

#ifndef SHELL_SHELLKEYBOARD_H
#define SHELL_SHELLKEYBOARD_H

#include <string>

class ShellKeyboard {
    static const char* ANSI_MOVE_UP;
    static const char* ANSI_MOVE_DOWN;
    static const char* ANSI_MOVE_LEFT;
    static const char* ANSI_MOVE_RIGHT;
    static const char* ANSI_CLEAR_LINE;
    static const char* ANSI_CURSOR_ROW;
    static const char* ANSI_CURSOR_COL;
    static const char* ANSI_SET_CURSOR;
    static const char* ANSI_SAVE_CURSOR_POS;
    static const char* ANSI_RESTORE_CURSOR_POS;
    static const char* ANSI_FOREGROUND;
    static const char* ANSI_CLEAR_SCREEN;

public:

    static void putCharSequence(const char* sequence);

    static void backspace();
    static void cursor_left(const uint8_t& amount = 1);
    static void cursor_right(const uint8_t& amount = 1);
    static void cursor_up(const uint8_t& amount = 1);
    static void cursor_down(const uint8_t& amount = 1);
    static void clear_line();
    static void set_row(const uint8_t& pos = 1);
    static void set_col(const uint8_t& pos = 1);
    static void clear_screen();

    static void setCursor(const uint8_t& row = 0, const uint8_t& col = 0);
    static void save_cursor();
    static void restore_cursor();

    static void setColor(const uint8_t& color);
};


#endif //SHELL_SHELLKEYBOARD_H
