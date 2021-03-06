#ifndef _BuddyAllocator_h_                   // include file only once
#define _BuddyAllocator_h_
#include <iostream>
#include <vector>
#include <math.h>
using namespace std;
typedef unsigned int uint;

/* declare types as you need */

class BlockHeader{
public:
	// think about what else should be included as member variables
	bool free;
	uint block_size;  // size of the block
	BlockHeader* next; // pointer to the next block
};

class LinkedList{
	// this is a special linked list that is made out of BlockHeader s. 
public:
	BlockHeader* head;		// you need a head of the list
public:
	void insert (BlockHeader* b){	// adds a block to the list
		BlockHeader* current = head;

		//if there is nothing in the vector put it in
		if (head == 0 || head == nullptr)
		{
			head = b;
		}
		//if there is something in the vector already
		else 
		{
			b->next = head;
			head = b;
		}

	}

	BlockHeader* remove (BlockHeader* b){  // removes a block from the list
		BlockHeader* current_Head = head;

		if(head == b)
		{
			head = head->next;
			return current_Head;
		}
		else 
		{
			BlockHeader* temp = nullptr;
			while(current_Head->next != nullptr){
				if (current_Head->next == b)
				{
					temp = current_Head->next;
					current_Head->next = current_Head->next->next;
					return temp;
				}
			}
			
		}
		return nullptr;
	}

	//Simple function to get the current head
	BlockHeader* getHead()
	{
		return head;
	}

	//Another remove function to remove the current head when not 
	//given a block address
	BlockHeader* remove(){
		BlockHeader* temp = head;

		if(temp != nullptr)
		{
			head = head->next;
			return temp;
		}

		return nullptr;
	}

};


class BuddyAllocator{
private:
	/* declare more member variables as necessary */
	vector<LinkedList> FreeList;
	int basic_block_size;
	int total_memory_size;

	char* mem_start;

private:
	/* private function you are required to implement
	 this will allow you and us to do unit test */
	
	BlockHeader* getbuddy (BlockHeader * addr){
		//using the formula given in the slides
		return (BlockHeader*)((((char *)addr - mem_start) ^ addr->block_size) + mem_start);


	}; 
	// given a block address, this function returns the address of its buddy 
	
	bool arebuddies (BlockHeader* block1, BlockHeader* block2)
	{
		//if both blocks are free and they are both the same size
		if (block1->free && block2->free)
		{
			if (block1->block_size == block2->block_size)
			{
				return true;
			}
			else
			{
				return false;
			}
			
		}
		else
		{
			return false;
		}
		
	};
	// checks whether the two blocks are buddies are not
	// note that two adjacent blocks are not buddies when they are different sizes

	BlockHeader* merge (BlockHeader* block1, BlockHeader* block2)
	{
		int point = FreeList.size() - log2(total_memory_size/block1->block_size) - 1;
		

		if(block1 > block2)
		{
			//remove both blocks from the FreeeList 
			FreeList[point].remove(block1);
			FreeList[point].remove(block2);

			//get rid of block1
			//delete block1;(im dumb)

			//set block2 to the new values
			block2->block_size = block2->block_size * 2;
			block2->free = true;
			block2->next = nullptr;

			//set new block2 back to the user
			return block2;
		}
		else
		{
			//remove both blocks from the FreeeList 
			FreeList[point].remove(block1);
			FreeList[point].remove(block2);

			//get rid of block2
			//delete block2;(same here)

			//set block1 to new values
			block1->block_size = block1->block_size * 2;
			block1->free = true;
			block1->next = nullptr;

			//return block1 to the user
			return block1;
		}		
	};
	// this function merges the two blocks returns the beginning address of the merged block
	// note that either block1 can be to the left of block2, or the other way around

	BlockHeader* split (BlockHeader* block){
		int new_Size = block->block_size/2;

		//make sure it is bigger than the basic block size
		if(new_Size >= basic_block_size)
		{
			//remove the block from the linkedlist
			int point = FreeList.size() - log2(total_memory_size/block->block_size)-1;
			FreeList[point].remove(block);

			//find the block size and make a new pointer at half point
			int half_Block_Size = block->block_size / 2;
			char *half_Block = (char *)block;
			half_Block = half_Block + half_Block_Size;
			BlockHeader *half_Block_Ptr = (BlockHeader*)half_Block;

			//Print the new Block Size for me
			//cout << "New Splitted Block " << block->block_size << endl << endl;

			//initialize the half_Block_Ptr's header
			half_Block_Ptr->block_size = block->block_size/2;
			half_Block_Ptr->free = true;
			half_Block_Ptr->next = nullptr;

			//insert it into the linked list
			FreeList[point - 1].insert(half_Block_Ptr);

			//making sure block have the new values and send it to the user
			block->block_size = block->block_size / 2;
			block->free = false;
			block->next = nullptr;


			//Print the new Block Size for me
			//cout << "Splitting New Block: " << half_Block_Ptr->block_size << endl << endl;

			return block;
		}
		return nullptr;
	};
	// splits the given block by putting a new header halfway through the block
	// also, the original header needs to be corrected


public:
	BuddyAllocator (int _basic_block_size, int _total_memory_length); 
	/* This initializes the memory allocator and makes a portion of 
	   ’_total_memory_length’ bytes available. The allocator uses a ’_basic_block_size’ as 
	   its minimal unit of allocation. The function returns the amount of 
	   memory made available to the allocator. 
	*/ 

	~BuddyAllocator(); 
	/* Destructor that returns any allocated memory back to the operating system. 
	   There should not be any memory leakage (i.e., memory staying allocated).
	*/ 

	void* alloc(int _length); 
	/* Allocate _length number of bytes of free memory and returns the 
		address of the allocated portion. Returns 0 when out of memory. */ 

	void free(void* _a); 
	/* Frees the section of physical memory previously allocated 
	   using alloc(). */ 
   
	void printlist ();
	/* Mainly used for debugging purposes and running short test cases */
	/* This function prints how many free blocks of each size belong to the allocator
	at that point. It also prints the total amount of free memory available just by summing
	up all these blocks.
	Aassuming basic block size = 128 bytes):

	[0] (128): 5
	[1] (256): 0
	[2] (512): 3
	[3] (1024): 0
	....
	....
	 which means that at this point, the allocator has 5 128 byte blocks, 3 512 byte blocks and so on.*/
};

#endif
