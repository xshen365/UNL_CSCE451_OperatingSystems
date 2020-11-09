#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <string>

using namespace::std;
/****************************************************************
 *
 * Author: Xingjian Shen
 * Title: PA5
 * Date: April 11, 2020
 * Description: (Hopefully helpful pseudocode) Template for PA5
 *
 ****************************************************************/


/******************************************************
 * Declarations
 ******************************************************/
// #Define'd sizes
const int PAGE_TABLE_SIZE = 256;
const int PM_SIZE = 256;
const int TLB_SIZE = 16;
int fault, hit = 0;

// TLB:
// Has an entry and value struct or array or array of structs
int tlb[TLB_SIZE][2];
int tlb_index = 0;

// Page table of ints (could be 1D or 2D depending on your preference)
int PAGE_TABLE[PAGE_TABLE_SIZE][2];
int page_index = 0;

// Memory array (could be 1D or 2D depending on your preference)
char PHYSICAL_MEM[PM_SIZE][PM_SIZE];

/******************************************************
 * Function Declarations
 ******************************************************/

/***********************************************************
 * Function: get_frame_TLB - tries to find the frame number in the TLB
 * Parameters: page_num
 * Return Value: the frame number, else NOT_FOUND if not found
 ***********************************************************/
int get_frame_TLB(int page_num) {
    for(int i = 0; i < TLB_SIZE; i++) {
        if(tlb[i][0] == page_num) {
            hit++;
            return tlb[i][1];
        }
    }
    return -1;
}
/***********************************************************
 * Function: get_available_frame - get a valid frame
 * Parameters: none
 * Return Value: frame number
 ***********************************************************/
int get_available_frame(int updated_frame) {
    return updated_frame;
}
/***********************************************************
 * Function: get_frame_pagetable - tries to find the frame in the page table
 * Parameters: page_num
 * Return Value: page number, else NOT_FOUND if not found (page fault)
 ***********************************************************/
int get_frame_pagetable(int page_num) {
    for(int i = 0; i < PAGE_TABLE_SIZE; i++) {
        if(page_num == PAGE_TABLE[i][0]) {
            return PAGE_TABLE[i][1];
        }
    }
    return -1;
}
/***********************************************************
 * Function: backing_store_to_memory - finds the page in the backing store and
 *   puts it in memory
 * Parameters: page_num - the page number (used to find the page)
 *   frame_num - the frame number for storing in physical memory
 * Return Value: none
 ***********************************************************/
void backing_store_to_memory(int page_num, int frame_num, const char *fname) {
//open file, "rb", fseek()-> find frame,
    char buff[PM_SIZE];
    FILE* store = fopen(fname, "rb");
    if(store == NULL) {
        cerr << "Store file not found!" << endl;
        exit(0);
    }
    if(fseek(store, page_num*PM_SIZE, SEEK_SET) != -1) {
        if(fread(buff, sizeof(char), PM_SIZE, store) != 0) {
            for(int i = 0; i < PM_SIZE; i++) {
                PHYSICAL_MEM[frame_num][i] = buff[i];
            }
        }
    }
    fclose(store);
}
/***********************************************************
 * Function: update_page_table - update the page table with frame info
 * Parameters: page_num, frame_num
 * Return Value: none
 ***********************************************************/
void update_page_table(int page_num, int frame_num) {
    PAGE_TABLE[page_index][1] = frame_num;
    PAGE_TABLE[page_index][0] = page_num;
    //page_index++;
    page_index = (page_index+1)%PAGE_TABLE_SIZE;
}
/***********************************************************
 * Function: update_TLB - update TLB (FIFO)
 * Parameters: page_num, frame_num
 * Return Value: none
 ***********************************************************/
void update_TLB(int page_num, int frame_num) {
    tlb[tlb_index][0] = page_num;
    tlb[tlb_index][1] = frame_num;
    tlb_index = (tlb_index+1)%TLB_SIZE;
}
/**
unsigned DecToBin(int decimal) {
    unsigned bin = 0;
    int remainder, temp = 1;
    while(decimal != 0) {
        remainder = decimal % 2;
        decimal /= 2;
        bin = bin+remainder*temp;
        temp *= 10;
    }
    return bin;
}
 **/

/******************************************************
 * Assumptions:
 *   If you want your solution to match follow these assumptions
 *   1. In Part 1 is is assumed memory is large enough to accommodate
 *      all frames -> no need for frame replacement
 *   2. Part 1 solution uses FIFO for TLB updates
 *   3. In the solution binaries it is assumed a starting point at frame 0,
 *      subsequently, assign frames sequentially
 *   4. In Part 2 you should use 128 frames in physical memory
 ******************************************************/

int main(int argc, char* argv[]) {
    // argument processing
    FILE* file;
    const char* backingStore;
    if(argc < 3) {
        cout << "Need more arguments!" << endl;
    }else {
        backingStore = argv[1];
        file = fopen(argv[2], "r");
    }

    FILE* output;
    output = fopen("correct.txt", "w");

    // For Part2: read in whether this is FIFO or LRU strategy

    // initialization
    int lineCount = 0;
    int updated_frame = 0;
    int phyAddress, value;
    // read addresses.txt
    char* line = NULL;
    size_t len = 0;

    // get new memories for tables
    memset(PAGE_TABLE, -1, sizeof(int)*PAGE_TABLE_SIZE*2);
    memset(PHYSICAL_MEM, -1, sizeof(PM_SIZE));
    memset(tlb, -1, sizeof(int)*TLB_SIZE*2);

    if(file == NULL) {
        exit(1);
    }

    while (getline(&line, &len, file) != -1) {
        lineCount++;
        int address = atoi(line); // integer address
        int page = (address>>8)&0xffff;
        int offset = address&0xff;
        int frame;
//        printf("%u %u\n", DecToBin(page), DecToBin(offset));

        frame = get_frame_TLB(page);
//        printf("frame line 191: %d page: %d\t", frame, page);
        if(frame == -1) { // TLB miss
            frame = get_frame_pagetable(page);
//            printf("frame line 194: %d page: %d\t", frame, page);
            if (frame == -1) { // page fault
                fault++;
                frame = get_available_frame(updated_frame);
                updated_frame++;
//                printf("frame line 198: %d page: %d\t", frame, page);
                backing_store_to_memory(page, frame, backingStore);
                update_page_table(page, frame);
            }

            // update tlb
            update_TLB(page, frame);
        }

        // get physical address and value
        phyAddress = (frame<<8)|offset;
//        printf("frame line 211: %d offset: %d\t\n\n", frame, offset);
        value = PHYSICAL_MEM[frame][offset];

        // pull addresses out of the file

        // Step 0:
        // get page number and offset
        //   bit twiddling

        // need to get the physical address (frame + offset):
        // Step 1: check in TLB for frame
        //   if !get_frame_TLB() -> :(
        //     Step 2: not in TLB, look in page table
        //     if !get_frame_pagetable() -> :(
        //       PAGE_FAULT!
        //       Step 3:
        //       dig up frame in BACKING_STORE.bin (backing_store_to_memory())
        //       bring in frame page# x 256
        //       store in physical memory
        //       Step 4:
        //       update page table with corresponding frame from storing
        //         into physical memory
        //   Step 5: (always) update TLB when we find the frame
        //     update TLB (updateTLB())
        //   Step 6: read val from physical memory

        fprintf(output, "Virtual address: %d Physical address: %d Value: %d\n", address, phyAddress, value);
//        printf("Virtual address: %d Physical address: %d frame: %d Value: %d\n",address, (frame << 8)|offset, frame, value);
    }
    if (line) {
        free(line);
    }
    fclose(file);

    // output useful information for grading purposes
    fprintf(output, "Number of Translated Addresses = %d\n", lineCount);
    fprintf(output, "Page Faults = %d\n", fault);
    fprintf(output, "Page Fault Rate = %.3f\n", (fault/(1.*lineCount)));
    fprintf(output, "TLB Hits = %d\n", hit);
    fprintf(output, "TLB Hit Rate = %.3f\n", (hit/(1.*lineCount)));
    fclose(output);
    return 0;
}
