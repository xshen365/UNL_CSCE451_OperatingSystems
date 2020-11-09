Instructions to compile: 

        make executable: make

        run: ./part2 -b 1 -p 5 -c 5 -i 10

        run: ./part2 -b 4 -p 30 -c 30 -i 10
        
        run: ./part2 -b 10 -p 1 -c 10 -i 20
        
        run: ./part2 -b 1000 -p 20 -c 20 -i 10000

        test: ./part2 -b 1 -p 5 -c 5 -i 10 | grep p: | wc -l

        test: ./part2 -b 1000 -p 20 -c 20 -i 10000 | grep p: | wc -l


Monitor:

For this part2, I used "signal and wait" paradigm. Since the problem itself is to let one thread or process produces or consumes one product at a time, "signal and wait" would fit this trait. If using "signal and continue", one thread would keep producing or consuming products all the time until all products are produced or used up, making other threads always queueing up, which conflicts the fact that multithreads or multiprocesses are used for monopolizing computer resources efficiently.


