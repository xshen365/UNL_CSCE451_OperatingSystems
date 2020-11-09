/****************************************************************
 *
 * Author: 
 * Title: 
 * Date: 
 * Description: (Hopefully helpful pseudocode) Template for PA5
 *
 ****************************************************************/


/******************************************************
 * Declarations
 ******************************************************/
// #Define'd sizes

// Make the TLB array
// Need pages associated with frames (could be 2D array, or C++ list, etc.)

// Make the Page Table
// Again, need pages associated with frames (could be 2D array, or C++ list, etc.)

// Make the memory
// Memory array (easiest to have a 2D array of size x frame_size)

/******************************************************
 * Function Declarations
 ******************************************************/

/***********************************************************
 * Function: get_page_and_offset - get the page and offset from the logical address
 * Parameters: logical_address
 *   page_num - where to store the page number
 *   offset - where to store the offset
 * Return Value: none
 ***********************************************************/
void get_page_and_offset(int logical_address, int *page_num, int *offset);

/***********************************************************
 * Function: get_frame_TLB - tries to find the frame number in the TLB
 * Parameters: page_num
 * Return Value: the frame number, else NOT_FOUND if not found
 ***********************************************************/
int get_frame_TLB(int page_num);

/***********************************************************
 * Function: get_available_frame - get a valid frame
 * Parameters: none
 * Return Value: frame number
 ***********************************************************/
int get_available_frame();

/***********************************************************
 * Function: get_frame_pagetable - tries to find the frame in the page table
 * Parameters: page_num
 * Return Value: page number, else NOT_FOUND if not found (page fault)
 ***********************************************************/
int get_frame_pagetable(int page_num);

/***********************************************************
 * Function: backing_store_to_memory - finds the page in the backing store and
 *   puts it in memory
 * Parameters: page_num - the page number (used to find the page)
 *   frame_num - the frame number for storing in physical memory
 * Return Value: none
 ***********************************************************/
void backing_store_to_memory(int page_num, int frame_num, const char *fname);

/***********************************************************
 * Function: update_page_table - update the page table with frame info
 * Parameters: page_num, frame_num
 * Return Value: none
 ***********************************************************/
void update_page_table(int page_num, int frame_num);

/***********************************************************
 * Function: update_TLB - update TLB (FIFO)
 * Parameters: page_num, frame_num
 * Return Value: none
 ***********************************************************/
void update_TLB(int page_num, int frame_num);


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

int main(int argc, char * argv[]) {
		// argument processing
		// For Part2: read in whether this is FIFO or LRU strategy

		// initialization
		
        // read addresses.txt
		while(addresses.txt not empty) {
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

		}

		// output useful information for grading purposes
		
}
