Instructions to compile: 

    make executable: make

    run: ./part1 -b 1 -p 5 -c 5 -i 10
        
    run: ./part1 -b 1000 -p 20 -c 20 -i 10000
        
    test: ./part1 -b 1 -p 5 -c 5 -i 10 | grep p: | wc -l
    
    test: ./part1 -b 1000 -p 20 -c 20 -i 10000 | grep p: | wc -l


Monitor:

For this part1, the logic is that I have a mutex sepmaphore to lock or unlock the waiting queue for processes or threads. There is a while loop in both producer and consumer function to let processes to wait for a full or empty semaphore, at the same time checking if the current buffer is at a appropriate position to produce(less than total item and mutex is 0) or consume(buffer is not empty). This makes sure that the lock is held one at a time by processes so the processes are forced into a waiting queue to unlock. This is a "signal and wait" paradigm, because whenever one process or thread is done with its current work it unlocks the door immediately and checks opon the queue to see if there are other waiting processes.


