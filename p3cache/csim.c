#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

#include "cachelab.h"


/*************************
 * Structure Definitions *
 *************************/


// valid is 0 if the line is empty, 1 if the line has stuff
// tag: each line has a tag and a unique id
typedef struct {
	int valid;
	int time_stamp;
	long long tag;
} cache_line_s;

// cache_lines is a pointer to an array of pointers
// with each pointing to a cache_line_s struct
typedef struct {
	cache_line_s **cache_lines;
} cache_set_s;

// cache_set is a pointer that points to an array of pointers
// with each pointing ot a cache_set_s struct
typedef struct {
	cache_set_s **cache_sets;
} cache_s;

// one struct to keep track of the hit/miss/eviction counts
typedef struct {
	int hitCount;
	int missCount;
	int evictionCount;
} result_s;


// mallocs one deep copy of cache_line
// returns a pointer to that deep copy of cache_line
cache_line_s *mk_empty_cache_line() {
	cache_line_s *c_line = (cache_line_s *)malloc(sizeof(cache_line_s));
	c_line -> valid = 0;
	c_line -> tag = -1;
	c_line -> time_stamp = -1;
	return c_line;
}

cache_set_s *mk_empty_cache_set(int E) {
	// make the cache_lines
	// a pointer that points to E (cache_lines_s *) -- a pointer to E pointers that each point to a line
	cache_line_s **c_lines = (cache_line_s **)malloc(sizeof(cache_line_s *) * E);
	int i;
	for (i = 0; i < E; i++) {
		c_lines[i] = mk_empty_cache_line();
	}

	// malloc the struct for the set and string everything together
	cache_set_s *c_set = (cache_set_s *)malloc(sizeof(cache_set_s));
	c_set -> cache_lines = c_lines;

	return c_set;
}

cache_s *mk_empty_cache(int E, int S) {
	// make the cache_sets: there are S sets in total
	// **c_sets is a pointer to S (cache_set_s*) -- same idea as above
	cache_set_s **c_sets = (cache_set_s **)malloc(sizeof(cache_set_s *) * S);
	int i;
	for (i = 0; i < S; i++) {
		c_sets[i] = mk_empty_cache_set(E);
	}
	// malloc the cache
	cache_s *c = (cache_s *)malloc(sizeof(cache_s));
	c -> cache_sets = c_sets;
	return c;
}


// we will only use this function when the lines are full
int find_lru_index(cache_set_s *c_set, int E) {
	int lru = (c_set -> cache_lines)[0] -> time_stamp;
	int lru_index = 0;

	int i;
	for (i = 1; i < E; i++) {
		if (((c_set -> cache_lines)[i] -> time_stamp) < lru) {
			lru = (c_set -> cache_lines)[i] -> time_stamp;
			lru_index = i;
		}
	}
	return lru_index;
}


// we only enter this function when we need to evict and write
// the case where we only need to write (there are still empty slots in the lines) is simple and covered straight up
void update_cache(cache_set_s *c_set, int E, long long tag_bit, int t_stamp) {
	int old_index = find_lru_index(c_set, E);
	(c_set -> cache_lines)[old_index] -> tag = tag_bit;
	(c_set -> cache_lines)[old_index] -> time_stamp = t_stamp;
	return;
}


void simulator(int t, int s, int b, int S, int E, unsigned long long address,
			   cache_s* cache, result_s *result, int t_stamp) {
	// get tag bit and set bit
	long long tag_mask = 1ll << 63 >> (t - 1);
	long long tag_bit = (address >> (s + b));
	long long set_bit = (address & (~tag_mask)) >> b;

	cache_set_s *c_set = (cache -> cache_sets)[set_bit];

	// run through the lines within your set now
	int i;
	for (i = 0; i < E; i++) {
		// we read and write each line in order
		cache_line_s *c_line = (c_set -> cache_lines)[i];

		if (c_line -> valid == 0) {
			result -> missCount++;
			// update cache
			c_line -> valid = 1;
			c_line -> tag = tag_bit;
			c_line -> time_stamp = t_stamp;
			break;
		}

		// valid bit is turned on, check the tag bit now:
		else {
			// tag bit matches, so it is a hit, update hitCount and time list
			if (c_line -> tag == tag_bit) {
				result -> hitCount++;
				c_line -> time_stamp = t_stamp;
				break;
			}
			// tag big doesn't match, do nothing except for if it is the last line
			else {
				// last line, update missCount and eviction count, update time list
				if (i == E - 1) {
					result -> missCount++;
					result -> evictionCount++;
					// update cache (time_stamp is also updated here)
					update_cache(c_set, E, tag_bit, t_stamp);
				}
			}
		}
	}
	return;
}

int main(int argc, char *argv[]) {
	// malloc a result structure to store hit/miss/eviction counts
	result_s *result = (result_s *)malloc(sizeof(result_s));
	// initialize all the values to 0
	result -> hitCount = 0;
	result -> missCount = 0;
	result -> evictionCount = 0;

	// variables for cache properties
	int t, s, b, S, E;
	// file name
	char *trace_file_name;
	// parsing arguments from command line
	char c;
	while((c = getopt(argc, argv, "s:E:b:t:")) != -1) {
		switch (c) {
			case 's':
				s = atoi(optarg);
				break;	
			case 'E':
				E = atoi(optarg);
				break;
			case 'b':
				b = atoi(optarg);
				break;
			case 't':
				trace_file_name = optarg;
				break;
			default:
				printf("./csim failed to parse its options.\n");
				exit(1);
		}
	}
	// 2^s = S: # of sets
	// 2^b = B: block size in bytes
	S = (1 << s);
	// tag bit size
	t = 64 - (s + b);
	

	// open file for reading, remember to close it later
	FILE *trace_file;
	trace_file = fopen(trace_file_name, "r");
	// check if file is open
	if (!trace_file) {
		printf("Couldn't open file %s for reading.\n", trace_file_name);
		exit(1);
	}
	
	// create the empty cache with specified E and S
	cache_s *cache = mk_empty_cache(E, S);

	char* operation;
	unsigned long long address;

	// the greater the time_stamp is, the more recent something is
	int time_stamp = 0;

	// ap stores the address string 
	char *ap;
	char line_p[100];
	// fills in every slot with the terminating character
	memset(line_p, '\0', 100);
	// fgets reads each line as a string, parse it through afterwards
	while (fgets(line_p, 100, trace_file) != NULL) {
		// make sure that we don't deal with Instructions
		if (line_p[0] == 'I') continue;

		// parse through the string now
		operation = strtok(line_p, " ");
		ap = strtok(NULL, ",");
		// convert hex string to long long unsigned
		address = strtoull(ap, NULL, 16);
		
		// the operation actually does not matter here - only when it is M,
		// you feed it into the simulator twice
		switch(operation[0]) {
			case 'L':
				simulator(t, s, b, S, E, address, cache, result, time_stamp);
				time_stamp++;
				break;
			case 'S':
				simulator(t, s, b, S, E, address, cache, result, time_stamp);
				time_stamp++;
				break;
			case 'M':
				simulator(t, s, b, S, E, address, cache, result, time_stamp);
				time_stamp++;
				simulator(t, s, b, S, E, address, cache, result, time_stamp);
				time_stamp++;
				break;
			default:
				printf("operation failed\n");
				exit(1);
		}
	}
	// close the file now
	fclose(trace_file);
	
	int hitCount = result -> hitCount;
	int missCount = result -> missCount;
	int evictionCount = result -> evictionCount;
	printSummary(hitCount, missCount, evictionCount);
    return 0;
}

