//
// Created by lars on 03.05.20.
//

#include "MiniShell.h"

#include <iostream>
#include <sstream>
#include <cstring>
#include <list>
#include <stdio.h>
#include <sys/stat.h>
#include <pwd.h>
#include <sys/types.h>
#include <unistd.h>
#include <wait.h>


#define STDIN_FILENO 0


#include "MiniShell.h"



/* Constructor
 * Enables getting key inputs directly e.g. Arrow-Keys
 */
MiniShell::MiniShell() {
    // init variables
    historyIndex = -1;

    // init working directory to users home directory
    char* homedir;
    if ((homedir = getenv("HOME")) == NULL){
        homedir = getpwuid(getuid())->pw_dir;
    }
    wd = homedir;

    // add builtin functions to map
    std::function<int(char**)> cd( std::bind(&MiniShell::builtin_cd, this, std::placeholders::_1));
    std::function<int(char**)> clear( std::bind(&MiniShell::builtin_clear, this, std::placeholders::_1));
    std::function<int(char**)> ls( std::bind(&MiniShell::builtin_ls, this, std::placeholders::_1));
    builtIns["cd"] = cd;
    builtIns["clear"] = clear;
    builtIns["ls"] = ls;


    // interupt keyboard input to console window
    static struct termios  oldt, newt;
    tcgetattr( STDIN_FILENO, &oldt);
    this->old_settings = oldt;
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr( STDIN_FILENO, TCSANOW, &newt);
}

/* Destructor
 * puts console settings back to normal
 */
MiniShell::~MiniShell() {
    tcsetattr( STDIN_FILENO, TCSANOW, &this->old_settings);
}


/* main loop
 * starts the shell
 */
void MiniShell::run() {
    running = true;
    while(running){
        std::string line = this->readLine(); // get user input
        bool waitFlag = true; // & at the end of the command will enable background exec
        if(line[line.size()-1] == '&'){
            line.pop_back();
            waitFlag = false;
        }

        char** args = this->parseLine(line); // parse
        execute(args, waitFlag);// execute
    }
}

/*******************************************************
 *                       User IO                       *
 *                                                     *
 *******************************************************/

/* Gets line from console
 *
 */
std::string MiniShell::readLine() {
    // interupt direct input to shell
    static struct termios  oldt, newt;
    tcgetattr( STDIN_FILENO, &oldt);
    this->old_settings = oldt;
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr( STDIN_FILENO, TCSANOW, &newt);

    if(commandHistory.size() == 0){
        commandHistory.emplace_back(std::list<char>());
        historyIndex = commandHistory.size()-1;
    }
    else if(historyIndex +1 == commandHistory.size()){
        commandHistory.emplace_back(std::list<char>());
        historyIndex = commandHistory.size()-1;
    }else{
        historyIndex = commandHistory.size()-1;
    }
    printWd();


    std::list<char>* command = currentCommand();
    int cursorPosition = 0;
    int c =0;


    // input loop
    while(true){

        /* debug code
        ShellKeyboard::save_cursor();
        ShellKeyboard::setCursor(0,0);
        ShellKeyboard::clear_line();
        std::cout << cursorPosition << "; " << command->size();
        ShellKeyboard::restore_cursor();
        */
        c = getchar();
        // Escape Character
        if(c == 27){
            if((c=getchar())= 91){
                switch((c=getchar())){
                    case 65:
                        if(historyIndex - 1 >= 0){ // arrow up
                            historyIndex--;
                            command = &commandHistory[historyIndex];
                            ShellKeyboard::cursor_left(cursorPosition + wd.size() + 1);
                            ShellKeyboard::clear_line();
                            cursorPosition = 0;

                            printWd();

                            for(auto it = command->begin(); it != command->end(); ++it){
                                putchar(*it);
                                cursorPosition++;
                            }
                        }

                        break;
                    case 66:
                        if(historyIndex + 1 <= commandHistory.size()-1){ // arrow down
                            historyIndex++;
                            command = &commandHistory[historyIndex];
                            ShellKeyboard::cursor_left(cursorPosition + wd.size() +1);
                            ShellKeyboard::clear_line();
                            cursorPosition = 0;

                            printWd();
                            for(auto it = command->begin(); it != command->end(); ++it){
                                putchar(*it);
                                cursorPosition++;
                            }
                        }
                        break;
                    case 67: //arrow right
                        if(cursorPosition+1 <= command->size()){
                            cursorPosition++;
                            ShellKeyboard::cursor_right();
                        }

                        break;
                    case 68: // arrow left
                        if(cursorPosition -1 >= 0){
                            cursorPosition--;
                            ShellKeyboard::cursor_left();

                        }
                        break;
                    default:
                        std::cout << int(c) << " Unknown escape sequence\n";
                        break;
                }
            }


        }else if(c == 127){ // Backspace
            if(cursorPosition == 0){
                // do nothing
            }else if(cursorPosition == command->size()){
                command->pop_back();
                ShellKeyboard::backspace();
                cursorPosition--;
            }else{
                ShellKeyboard::save_cursor();

                auto index = command->begin();
                std::advance(index, cursorPosition-1);
                command->erase(index); // delete l

                ShellKeyboard::clear_line();
                ShellKeyboard::cursor_left(cursorPosition-- + wd.size() +1);
                printWd();

                for(auto it = command->begin(); it != command->end(); ++it){
                    putchar(*it);
                }
                ShellKeyboard::restore_cursor();
                ShellKeyboard::cursor_left();
            }
        }
        else if(c == '\n'){ // enter (carriage return)
            putchar('\n');
            std::string ret = "";

            if(historyIndex +1 < commandHistory.size()){
                historyIndex = commandHistory.size()-1;
                commandHistory[historyIndex] = *command;
                command = currentCommand();
            }


            for(auto it = command->begin(); it != command->end(); ++it){
                ret.push_back(*it);
            }


            return ret;
        }else if( c == '\t'){ // tabulator
            for( int i=0; i< tabSize; i++) {
                command->push_back( ' ');
                cursorPosition++;
                putchar(' ');
            }

        }else{ // normal letters
            if(cursorPosition !=  command->size()){
                ShellKeyboard::save_cursor();

                auto index = command->begin();
                std::advance(index, cursorPosition);
                command->emplace(index, c);

                ShellKeyboard::clear_line();
                ShellKeyboard::cursor_left(++cursorPosition + wd.size() + 1);
                printWd();

                for(auto it = command->begin(); it != command->end(); ++it){
                    putchar(*it);
                }
                ShellKeyboard::restore_cursor();
                ShellKeyboard::cursor_right();
            }else{
                command->push_back(c);
                cursorPosition++;
                putchar(c);
            }
        }


    }



}

/* splits the given string by the given delimiter
 * returns a char pointer array containing the segments with the last value being NULL
 */
char **MiniShell::parseLine(std::string line,const char& deliminator) {
    std::stringstream text(line);
    std::string seg;
    std::vector<std::string> segments;
    char** ret;

    while(std::getline(text, seg, deliminator)){
        segments.push_back(seg);
    }

    ret = new char*[segments.size() + 1];
    int i=0;
    for(; i<segments.size(); ++i){
        ret[i] = new char[segments[i].length()];
        strcpy(ret[i], segments[i].c_str());
    }
    ret[i] = NULL;
    return ret;
}

std::list<char> *MiniShell::currentCommand() {
    return &commandHistory[historyIndex];
}

void MiniShell::printWd() {
    ShellKeyboard::setColor(32);
    std::cout << wd << '>';
    ShellKeyboard::setColor(37);
}

/*******************************************************
 *                   programm funcs                    *
 *                                                     *
 *******************************************************/


int MiniShell::execute(char ** args, bool waitFlag) {
    if (args[0] == NULL){
        return 1;
    }

    auto search = builtIns.find(std::string(args[0]));
    if( search != builtIns.end() ){
        return search->second(args);
    }
    // grundgerüst

    //std::cout << makeAbsolute(args[0]) << '\n';
    int res = checkFile(makeAbsolute(args[0]).c_str(), false);
    switch(res){
        case -1: // is not a file -> search for command
            startProgramm(args, waitFlag);
            break;
        case 0: // is a directory
            std::cout << "Is a directory\n";
            break;
        case 1: // is a file
            std::cout << "Is a file\n";
            break;
        case 2: // is an executable file
            startProgramm(args, waitFlag);
            break;
    }



    return 0;


}

int MiniShell::checkFile(const char *path, bool verbose) {
    struct stat sb;
    int status;

    status = stat(path, &sb);

    if (lstat(path, &sb) == 0 && S_ISDIR(sb.st_mode))
    {
        if(verbose) std::cout << path << " is a directory.\n";
        return 0;

    } else if( lstat(path, &sb) == 0 && S_ISREG(sb.st_mode)){
        int ret = 1;
        if( lstat(path, &sb) == 0 && S_IXUSR){
            if(verbose) std::cout << path << " is an executable file.\n";
            ret = 2;
        }
        if(verbose) std::cout << path << " is a file.\n";
        return ret;
    }else{
        if(verbose) std::cout << path << " is nothing\n";
    }
    return -1;
}
/* starts a programm,
 * wait : if true, parent waits til child is teminated
 */
int MiniShell::startProgramm(char **args, bool wait) {
    pid_t pid, wpid;
    int status;

    pid = fork();
    if (pid == 0) {
        // Child process
        if (execvp(args[0], args) == -1) {
            perror(args[0]);
        }
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        // Error forking
        perror(args[0]);
        exit(EXIT_FAILURE);
    } else {
        // Parent process
        if(wait){ // wait for child to terminate
            do {
                wpid = waitpid(pid, &status, WUNTRACED);
            } while (!WIFEXITED(status) && !WIFSIGNALED(status) );
        }else{
            signal(SIGCHLD, SIG_IGN); // prevent zombie process
        }
    }
    return 1;
}

/* makes a given path absolut
 * e.g. convert ./... to wd/...
 *
 * TODO add ~ as homedir
 */
std::string MiniShell::makeAbsolute(char *path) {
    std::string abs_path;
    if(path[0] == '.'){ // relative path
        if (path[1] == '.'){ // back one
            abs_path = wd;
            if (abs_path[abs_path.size()-1] == '/') {
                abs_path.pop_back();
            }
            while(abs_path[abs_path.size()-1] != '/') {
                abs_path.pop_back();
            }

        }else{ // append path to wd
            abs_path = wd;
            int i = 1;
            if (abs_path[abs_path.size()-1] == '/') {
                abs_path.pop_back();
            }
            while(path[i] != 0){
                abs_path.push_back(path[i]);
                i++;
            }
        }

    }else{
        abs_path = path;
    }
    return abs_path;
}

/*******************************************************
 *                    builtin funcs                    *
 *                                                     *
 *******************************************************/

int MiniShell::builtin_cd(char **args) {
    if(args[1] == NULL){
        std::cout << "A directory must be given\n";
        return 0;
    }

    std::string path = makeAbsolute(args[1]);

    int res = checkFile(path.c_str(), false);
    if(res == 0) wd = path;
    else if(res == 1) std::cout << "Cant change directory to a file\n";
    else std::cout << path << " : could not be found\n";

    return 0;
}

/* TODO move cursor to home position
 *
 */
int MiniShell::builtin_clear(char **args) {

    ShellKeyboard::clear_screen();
    ShellKeyboard::setCursor();
    return 0;
}

int MiniShell::builtin_ls(char **args) {

    if (args[1] == NULL){
        char** new_args = new  char*[3];
        new_args[0]  = new char[3];
        strcpy(new_args[0], "ls");
        new_args[1] = new char[wd.size()];
        strcpy(new_args[1],wd.c_str());
        new_args[2] = new char;
        new_args[2] = NULL;
        startProgramm(new_args);
        for(int i = 0; i< 3; i++){
            delete new_args[i];
        }
        delete [] new_args;
    }else{
        startProgramm(args);
    }



}
