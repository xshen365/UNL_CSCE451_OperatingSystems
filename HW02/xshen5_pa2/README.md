This assignment is using the parser from the previous TA: Yutaka Tsutano

Instructions to compile: 
make executable: make
run: ./osh -t < testcase#.txt >& #out.t
test: diff #out.t ea#.txt
(if nothing is outputted given the above command it should work fine)

The problems I had for this assignment:
1. I had trouble closing pipes at the correct place to make the program work as expected.
2. I had trouble understanding pipes.
3. I had trouble handling previous errors that appear in PA2 which did not appear in PA1(moreLogical testcase).

What I learned in this assignment:
I have learned to understand how pipes work. I have learned before calling another pipe to open, we always need to close the writing end of the pipe in the parent process to continue the program, or the program would need an interruption on the command line to exit. I also learned to use dup2() to redirect between pipes and stdin and stdout. 



