//
// Created by lars on 03.05.20.
//

#ifndef SHELL_MINISHELL_H
#define SHELL_MINISHELL_H

#include <vector>
#include <list>
#include <string>
#include "ShellKeyboard.h"
#include <termios.h>
#include <map>
#include <functional>
#include <chrono>

class MiniShell {
public:
    MiniShell();
    ~MiniShell();
    void run();

private:
    // input settings
    int tabSize = 4;

    struct termios old_settings;
    bool running;
    std::string wd;
    int historyIndex;
    std::vector<std::list<char>> commandHistory;
    std::list<char>* currentCommand();

    // io functions
    std::string readLine();
    char **parseLine(std::string line, const char& deliminator=' ');
    void printWd();

    // BuiltIns
    std::map<std::string, std::function<int(char**)>> builtIns;
    int builtin_cd(char** args);
    int builtin_clear(char ** args);
    int builtin_ls(char ** args);

    int checkFile(const char* path, bool verbose = true);
    std::string makeAbsolute(char * path);
    int startProgramm(char** args, bool wait = true);
    int execute(char** args, bool waitFlag);
};


#endif //SHELL_MINISHELL_H
