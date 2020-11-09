/*
 * Copyright (c) 2018, Yutaka Tsutano
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
 * INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <string.h>
#include <vector>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fstream>
#include <fcntl.h>

#include "command.hpp"
#include "parser.hpp"


int myexec(const std::string& cmd, const std::vector<std::string>& args)
{
    // Make an ugly C-style args array.
    std::vector<char*> c_args = {const_cast<char*>(cmd.c_str())};
    for (const auto& a : args) {
        c_args.push_back(const_cast<char*>(a.c_str()));
    }
    c_args.push_back(nullptr);
    
    return execvp(cmd.c_str(), c_args.data());
}

int main(int agc, char* argv[])
{
    std::string dash_t;
    
    if (argv[1]) {
        dash_t = argv[1];
    }
    std::string input_line;
    
    for (int should_run = 0; should_run < 25; should_run++) {
        // Print the prompt.
        if ( dash_t=="-t" ) {
            
        }else {
            std::cout << "osh> " << std::flush;
        }
        
        // Read a single line.
        if (!std::getline(std::cin, input_line) || input_line == "exit") {
            break;
        }
        
        try {
            // Parse the input line.
            std::vector<shell_command> shell_commands
            = parse_command_string(input_line);
            
            //             Print the list of commands.
            //             std::cout << "-------------------------\n";
            bool succ = 1;
            int wstatus;
            for (const auto& cmd : shell_commands) {
                pid_t cpid = fork();
                
                //                std::cout << cmd;
                //                std::cout << "-------------------------\n";
                if (cpid < 0) {
                    fprintf(stderr, "Fork Failed \n");
                    exit(1);
                    
                }else if (cpid == 0) { //Child Process
                    
                    if (cmd.cin_mode == istream_mode::file) {
                        //                        FILE* input_file = fopen(cmd.cin_file.c_str(), "rw");
                        //                        int input_desc = fileno(input_file);
                        int input_desc = open(cmd.cin_file.c_str(), O_RDWR);
                        dup2(input_desc, 0);
                        close(input_desc);
                    }
                    
                    if (cmd.cout_mode == ostream_mode::append) {
                        int output_desc = open(cmd.cout_file.c_str(), O_APPEND);
                        dup2(output_desc, 1);
                        close(output_desc);
                    }
                    
                    
                    if (cmd.cout_mode == ostream_mode::file) {
                        //                        FILE* output_file = fopen(cmd.cout_file.c_str(), "rw");
                        //                        int output_desc = fileno(output_file);
                        int output_desc = open(cmd.cout_file.c_str(), O_CREAT | O_APPEND | O_RDWR, 0666);
                        dup2(output_desc, 1);
                        close(output_desc);
                    }
                    
                    
                    if(succ == 1) {
                        wstatus = myexec( cmd.cmd, cmd.args );
                    }
                    
                    exit(1);
                    
                }else { //Parent Process
                    wait(&wstatus);
                    int status = WEXITSTATUS(wstatus);
                    if (cmd.next_mode == next_command_mode::always) {
                        succ = 1;
                    }
                    
                    if (cmd.next_mode == next_command_mode::on_success) {
                        
                        if (status==0) {
                            succ = 1;
                        }else {
                            succ = 0;
                        }
                    }
                    if(cmd.next_mode == next_command_mode::on_fail) {
                        if (status==0) {
                            succ = 0;
                        }else {
                            succ = 1;
                        }
                    }
                    
                }
            }
        }
        catch (const std::runtime_error& e) {
            //            std::cout << "osh: " << e.what() << "\n";
            std::cout << e.what() << "\n";
        }
    }
    
    //    std::cout << std::endl;
}

