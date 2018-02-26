

//
// Programmer Name = Shah Jabeen Sajid
// File Name = csim.c
// Date = 16- February 2018
// Decription:
//           To rite a cache simulator in csim.c that takes a valgrind
// memory trace as input, simulates the hit/miss behavior of a cache
// memory on this trace, and outputs the total number of hits,
// misses, and evictions.
//

#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <strings.h>
#include <limits.h>

#include "cachelab.h"



typedef struct
{
   int set_bit;                       
   int block_bit;                     
   int E;                       
   int S;                       
   int B; 
   int hits;
   int misses;
   int evictions;
} cache_org;

struct cache_line
{
   int valid_bit;
   unsigned long int tag;
   int LRU_counter;
};


int verbosity;
unsigned long int set_index_mask=0;



void printUsage()
{
   printf( "Usage: [-hv] -s <num> -E <num> -b <num> -t <file>\n" );
   printf( "Options:\n" );
   printf( "  -h         Print this help message.\n" );
   printf( "  -v         Optional verbose flag.\n" );
   printf( "  -s <s>:    Number of set index bits.\n" );
   printf( "  -E <E>:    Associativity (number of lines per set).\n" );
   printf( "  -b <b>:    Number of block bits.\n" );
   printf( "  -t <tracefile>:  Name of the valgrind trace to replay.\n" );
   printf( "\nExamples:\n" );
   printf( " linux> ./csim -s 4 -E 1 -b 4 -t traces/yi.trace\n" );
   printf( " linux> ./csim -v -s 8 -E 2 -b 4 -t traces/yi.trace\n" );
  
}


int main( int argc, char **argv )
{
   cache_org input;;

   char *trace;
   char c;

   while ( ( c = getopt( argc, argv, "s:E:b:t:vh" ) ) != -1 )
   {
      switch ( c )
      {
	 case 's':
	    input.set_bit = atoi( optarg );
	    break;
	 case 'E':
	    input.E = atoi( optarg );
	    break;
	 case 'b':
	    input.block_bit = atoi( optarg );
	    break;
	 case 't':
	    trace = optarg;
	    break;
	 case 'v':
	    verbosity = 1;
	    break;
	 case 'h':
	    printUsage();
	    break;
	 default:
	    printUsage();
	    break;
      }
   }

   if ( input.set_bit == 0 || input.E == 0 || input.block_bit == 0 || trace == NULL )
   {
      printf( "%s: Invalid Input \n", argv[0] );
      printUsage( argv );
      exit( 1 );
   }

   
   //Compute S and B
   input.S = ( 1 << input.set_bit );
   input.B = ( 1 << input.block_bit );

  
   struct cache_line ** cache= NULL;
   
   cache = (struct cache_line**) malloc (input.S * sizeof (struct cache_line*));
   for (int i =0; i<input.S; i++)
   {
      cache[i] = (struct cache_line*) malloc (input.E* sizeof (struct cache_line));
   }
   set_index_mask = input.S -1;

     
   input.hits = 0;
   input.misses =0;
   input.evictions=0;
   /* TODO: Run the trace simulation */

   char identifier;             //L,S,M
   int size;                    //size read in from file
   int Rec_LRU = 0;             //value for LRU
   int empty = -1;              //index of empty space
   int hit_flag = 0;            //is there a hit
   int evict_flag = 0;          //is there an eviction
   unsigned long int address;

   //open the file and read it in
   FILE *traceFile = fopen( trace, "r" );

   if ( traceFile != NULL )
   {
      while ( fscanf( traceFile, " %c %lx,%d", &identifier, &address, &size ) >0 )
      {
	 // a Flag to know which input to evict.
	 int to_evict = 0;             
	 if ( identifier != 'I' )
	 {
	    
	    unsigned long int set_num = (address>> input.block_bit) & set_index_mask;
	    unsigned long tag = (address >>(input.block_bit + input.set_bit));
	    
	    int value = INT_MAX; 

	    for ( int e = 0; e < input.E; e++ )
	    {
	       if ( cache[set_num][e].valid_bit == 1 ) {
		  // Check hit before evicting any data
		  if ( cache[set_num][e].tag == tag ) {
		     input.hits++;
		     hit_flag= 1;
		     cache[set_num][e].LRU_counter = Rec_LRU;
		     Rec_LRU++;
		  }
		  // find the oldest element for eviction
		
		  else if ( cache[set_num][e].LRU_counter < value ) {
		     value = cache[set_num][e].LRU_counter;
		     to_evict = e;
		  }
	       }
	       // if nothing is set to empty then mark the one found
	       else if( empty == -1 ) {
		  empty = e;
	       }
	    }

	    // miss_ flag 
	    if ( hit_flag != 1 )
	    {
	       input.misses++;
	       //if we have an empty line
	       if ( empty > -1 )
	       {
		  cache[set_num][empty].valid_bit = 1;
		  cache[set_num][empty].tag = tag;
		  cache[set_num][empty].LRU_counter = Rec_LRU;
		  Rec_LRU++;
	       }
	       //if the set is full we need to evict
	       else if ( empty < 0 )
	       {
		  evict_flag = 1;
		  cache[set_num][to_evict].tag = tag;
		  cache[set_num][to_evict].LRU_counter = Rec_LRU;
		  Rec_LRU++; // increment counter here too
		  input.evictions++;
	       }
	    }
	    //if the instruction is M, we will always get a hit
	    if ( identifier == 'M' )
	    {
	       input.hits++;
	    }
	    //Print the result
	    if ( verbosity == 1 )
	    {
	       printf( "%c ", identifier );
	       printf( "%lx,%d", address, size );
	       if ( hit_flag == 1 )
	       {
		  printf( " Hit " );
	       }
	       else if ( hit_flag != 1 )
	       {
		  printf( " Miss " );
	       }
	       if ( evict_flag == 1 )
	       {
		  printf( " Eviction " );
	       }
	   
	       printf( "\n" );
	    }
	    empty = -1;
	    hit_flag = 0;
	    evict_flag = 0;
	 }
      }
   }

   /* Print out real results */
   printSummary( input.hits,input.misses, input.evictions );
   return 0;
}
