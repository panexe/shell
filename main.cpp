#include <iostream>
#include "MiniShell.h"
#include <signal.h>

std::chrono::time_point<std::chrono::high_resolution_clock> program_start;
MiniShell miniShell = MiniShell();

void signal_callback_handler(int signum) {
    std::cout << '\n';

    if(signum == 2){
        auto program_end = std::chrono::high_resolution_clock::now();

        auto time = std::chrono::duration<double, std::milli>(program_end- program_start);

        if(   time.count() / 1000 < 1){ //seconds
            if(time.count() / 1000 / 60 < 1 ){ // minutes
                if(time.count() / 1000 / 60 / 60 < 1){ // hours

                }else{
                    std::cout << time.count() / 1000 / 60 / 60 <<  " hours elapsed.\n";
                }
            }else{
                std::cout << time.count() / 1000 / 60 <<  " minutes elapsed.\n";
            }
        }else{
            std::cout << time.count() / 1000  <<  " seconds elapsed.\n";
        }

        std::cout << time.count() / 1000 << "seconds" << '\n';
    }

    miniShell.~MiniShell();
    // Terminate program
    exit(signum);
}


int main() {
    signal(SIGINT, signal_callback_handler);

    program_start = std::chrono::high_resolution_clock::now();

    miniShell.run();
    return 0;
}
