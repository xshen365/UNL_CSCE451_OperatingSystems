This assignment is using the parser from the previous TA: Yutaka Tsutano

Instructions to compile: 
make executable: make
run: ./osh -t < testcase#.txt > #out.t
(for test case 4: use ./osh -t < 4.logicalConditional.txt >& 4out.t)
test: diff #out.t ea#.txt
(if nothing is outputted given the above command it should work fine)

The problems I had for this assignment:
1. I had trouble redirecting output and input destination address correctly.
2. I had trouble understanding and using WIFEXTED function to return child process status.
3. Some basic understanding of C++ and shell command.

What I learned in this assignment:
I have learned how to use fork() to make a new child and I can look up its return status to check if the child process has exited norrmally or with a signal. I learned to use wait() to let parent process hold on to the child process to make the program execute in order. I also learned using dup2() to change stdout and stdin address and redirect output or input to correct place. Some exit() status such as exit(1) or exit(0) or exit(3) are useful to check the exit status. Difference among different exec() functions are useful to know to locate correct executables. The running order of parent and child process is also important to understand to make excution in order. From the assignment I was able to have better understanding of how shell command work in linux system and I also learned some basics in C++.


