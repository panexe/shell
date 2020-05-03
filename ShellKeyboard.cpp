//
// Created by lars on 03.05.20.
//

#include "ShellKeyboard.h"

#include <cstdio>
#include <algorithm>
#include "ShellKeyboard.h"

const char* ShellKeyboard::ANSI_MOVE_UP = "\033[1A";
const char* ShellKeyboard::ANSI_MOVE_DOWN = "\033[1B";
const char* ShellKeyboard::ANSI_MOVE_LEFT = "\033[1D";
const char* ShellKeyboard::ANSI_MOVE_RIGHT = "\033[1C";
const char* ShellKeyboard::ANSI_CLEAR_LINE = "\033[2K";
const char* ShellKeyboard::ANSI_CURSOR_ROW;
const char* ShellKeyboard::ANSI_CURSOR_COL;

const char* ShellKeyboard::ANSI_SET_CURSOR = "\033[R;CH";
const char* ShellKeyboard::ANSI_SAVE_CURSOR_POS = "\033[s";
const char* ShellKeyboard::ANSI_RESTORE_CURSOR_POS = "\033[u";
const char* ShellKeyboard::ANSI_FOREGROUND = "\033[com";
const char* ShellKeyboard::ANSI_CLEAR_SCREEN = "\033[2J";

void ShellKeyboard::backspace() {
    cursor_left();
    putchar(' ');
    cursor_left();
}

void ShellKeyboard::cursor_left(const uint8_t& amount) {
    uint8_t amount_left = amount;
    if(amount > 9){
        amount_left = amount - 9;
        ShellKeyboard::cursor_left(9);
        ShellKeyboard::cursor_left(amount_left);
        return;
    }

    if(amount != 1){
        std::string move_n = ANSI_MOVE_LEFT;
        std::replace(move_n.begin(), move_n.end(), '1', char(amount_left+ '0'));
        putCharSequence(move_n.c_str());
    }else{
        putCharSequence(ShellKeyboard::ANSI_MOVE_LEFT);
    }
}

void ShellKeyboard::cursor_right(const uint8_t& amount) {
    uint8_t amount_left = amount;
    if(amount > 9){
        amount_left = amount - 9;
        ShellKeyboard::cursor_right(9);
        ShellKeyboard::cursor_right(amount_left);
        return;
    }

    if(amount != 1){
        std::string move_n = ANSI_MOVE_RIGHT;
        std::replace(move_n.begin(), move_n.end(), '1', char(amount_left+ '0'));
        putCharSequence(move_n.c_str());
    }else{
        putCharSequence(ShellKeyboard::ANSI_MOVE_RIGHT);
    }
}

void ShellKeyboard::cursor_up(const uint8_t& amount) {
    uint8_t amount_left = amount;
    if(amount > 9){
        amount_left = amount - 9;
        ShellKeyboard::cursor_up(9);
        ShellKeyboard::cursor_up(amount_left);
        return;
    }

    if(amount != 1){
        std::string move_n = ANSI_MOVE_UP;
        std::replace(move_n.begin(), move_n.end(), '1', char(amount_left+ '0'));
        putCharSequence(move_n.c_str());
    }else{
        putCharSequence(ShellKeyboard::ANSI_MOVE_UP);
    }
}

void ShellKeyboard::cursor_down(const uint8_t& amount) {
    uint8_t amount_left = amount;
    if(amount > 9){
        amount_left = amount - 9;
        ShellKeyboard::cursor_down(9);
        ShellKeyboard::cursor_down(amount_left);
        return;
    }

    if(amount != 1){
        std::string move_n = ANSI_MOVE_DOWN;
        std::replace(move_n.begin(), move_n.end(), '1', char(amount_left+ '0'));
        putCharSequence(move_n.c_str());
    }else{
        putCharSequence(ShellKeyboard::ANSI_MOVE_DOWN);
    }
}

void ShellKeyboard::clear_line() {
    putCharSequence(ANSI_CLEAR_LINE);
}

/* puts a char sequence to the console
 * string must be terminated
 */
void ShellKeyboard::putCharSequence(const char *sequence) {
    int i = 0;
    while(sequence[i] != 0){
        putchar(sequence[i]);
        ++i;
    }
}

void ShellKeyboard::set_row(const uint8_t &pos) {
    return;
}

void ShellKeyboard::set_col(const uint8_t &pos) {
    return;
}

void ShellKeyboard::save_cursor() {
    putCharSequence(ANSI_SAVE_CURSOR_POS);
}

void ShellKeyboard::restore_cursor() {
    putCharSequence(ANSI_RESTORE_CURSOR_POS);
}

void ShellKeyboard::setCursor(const uint8_t &row, const uint8_t &col) {
    std::string set_pos = ANSI_SET_CURSOR;
    std::replace(set_pos.begin(), set_pos.end(), 'R', char(row+ '0'));
    std::replace(set_pos.begin(), set_pos.end(), 'C', char(col+ '0'));
    putCharSequence(set_pos.c_str());
}

void ShellKeyboard::setColor(const uint8_t &color) {
    if(color > 37 || color < 30){
        return;
    }

    std::string change_color = ANSI_FOREGROUND;

    std::replace(change_color.begin(), change_color.end(), 'c', char(color / 10 + '0'));
    std::replace(change_color.begin(), change_color.end(), 'o', char(color % 10 + '0'));
    putCharSequence(change_color.c_str());
}

void ShellKeyboard::clear_screen() {
    putCharSequence(ANSI_CLEAR_SCREEN);
}