#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <getopt.h>
#include <strings.h>

#include "cachelab.h"

int spt; // Search pointer

typedef struct{
	int s; // set number
	int e; // associativity
	int b; // offset number
	int nummiss;
	int numhit;
	int numevict;
}param;

typedef struct{
	long long set;
	long long tag;
	long long offset;
	long int index;
	int valid; // 0 means empty, 1 means set
}cacheline;

typedef struct {
	cacheline *c;
	int lru[1000][1000];
}cache;

typedef struct{
	long long set;
	long long tag;
	long long offset;
}trace; // reference trace


cache* c_construct(param *p) {
	int big_s = pow(2.0, p->s);
	cacheline *c = malloc(sizeof(cacheline) * big_s * p->e);
	cache *ch = malloc(sizeof(cache));
	//int lru[p->s][p->e];
	//ch->lru = lru;
	int i, j;
	int numline = big_s * p->e;
	int count = 0;
	for (i = 0; i < numline; i++) {
		c[i].valid = 0;
	}
	for (i = 0; i < numline; i += p->e) {
		for (j = 0; j < p->e; j++) {
			c[i+j].set = count;
			c[i+j].index = i+j+1; //number of each line (order in the cache rather than
			                      //the set)--hantaow
			ch->lru[i][j] = 0; //initialize all elements of lru to 0 --hantaow
		}
		count++;
	}
	ch->c = c;
	return ch;
}
/*
void free_cache(cache* ch, param* p){
	//int big_s = pow(2.0, p->s);
	//int numline = big_s * p->e;
	int i, j;
	//for(i = 0; i < numline; i++){
	//	free((ch->c)[i]);
	//}
//	for(i = 0; i < 1000; i++){
//		free((ch->lru)[i]);
	}
	for (i = 0; i < 1000; i++) {
		for (j = 0; j < 1000; j++) {
			free((ch->c)[i][j]);
		}
	}
	free(ch->c);
	free(ch->lru);
	free(ch);
}
*/

trace* t_parse(long long adr, param *p) {
	//printf("\ndebug: address = %llx\n", adr);
	trace *t = malloc(sizeof(trace));
	t->tag = adr & ((-1) << (p->b + p->s));
	t->set = ((adr & ((-1) << p->b)) & (~((-1) << (p->b + p->s)))) >> (p->b);
	t->offset = adr & (~((-1) << p->b));
	return t;
}

void free_trace(trace *t){
	free(t);
}

void lru_mod(int set_num, int line_ind, cache* ch, param *p){
	//int* the_set = lru[set_num][0];
	//int i = 0;
	int *t, *q;
	//int e = p->e;
	//int temp;
	t = &(ch->lru)[set_num][0]; 
	q = &(ch->lru)[set_num][0];
	while (*t != 0 /*&& i != e*/) {
		//i++;
		t = t/*lru[set_num][0]*/ + 1;//*i;
	}
	t = t - 1;
	//i = 1;
	while (*q != line_ind) {
		q = q/*lru[set_num][0]*/ + 1;
	}
	/*if (i >= e) {
		printf("debug: i = %d, e = %d\n", i, e);
		fprintf(stderr, "no the indicated line in the lru");
		exit(0);
	}*/
	if (q == t) {
		return;
	} else if (q <= t) {
		while (q <= t) {
			*q = *(q + 1);
			q++;
		}
		*t = line_ind;
		return;
	} else {
		printf("\ndebug: &q = %p, &t = %p\n", q, t);
		fprintf(stderr, "there must be something wrong\n");
		exit(0);
	}
}

void lru_fill(int set_num, int line_ind, cache* ch, param *p){
	//int* the_set = lru[set_num];
	int i;
	for (i = 0; i < p->e; i++) {
		if ((ch->lru)[set_num][i] == 0) {
			(ch->lru)[set_num][i] = line_ind;
			return;
		}
	}
	//fprintf(stderr, "lru is full!!!");
	//exit(0);
}

void c_write(cache *ch, int line_num, trace *t) {
	(ch->c[line_num]).set = t->set;
	(ch->c[line_num]).tag = t->tag;
	(ch->c[line_num]).offset = t->offset;
	(ch->c[line_num]).valid = 1;
}

void c_evict(cache *ch, trace *t, int set_num, param *p) {
	//int* the_set = ch->lru[set_num];
	int evl_ind = (ch->lru)[set_num][0];
	c_write(ch, evl_ind-1, t);
	lru_mod(set_num, evl_ind, ch, p);
}

//void all_free()

void c_search(cache *ch, trace *t, param *p) {
	cacheline* c = ch->c;
	int big_s = pow(2.0, p->s);
	//int** lru = ch->lru;
	int i, j;
	int numline = big_s * p->e;
	int e = p->e;
	//printf("\ndebug: t->set = %lld\n", t->set);
	for (i = 0; i < numline; i++) {
		if (t->set == c[i].set) {
			//printf("\ndebug: t->set = %lld, c[i].set = %lld\n", t->set, c[i].set);
			for (j = 0; j < e && t->set == c[i+j].set; j++) {
				if (t->tag == c[i+j].tag && (c[i+j].valid == 1)) {
					//printf(" entering hit!");
					printf(" hit");
					p->numhit++;
					//return 1; // Hit
					/*needs a function to modify lru!!!!!!*/
					lru_mod(c[i].set, c[i+j].index, ch, p);
					//printf(" hit");
					return;
				} else if (/*t->tag != c[i+j].tag &&*/ c[i+j].valid == 0){
					printf(" miss");
					//printf(" entering miss!");
					c_write(ch, i+j, t);
					p->nummiss++;
					//return 0; // Miss, Needs to fill in the block (write)
					/*needs a function to fill in lru!!!!!!*/
					lru_fill(c[i].set, c[i+j].index, ch, p);
					//printf(" miss");
					return;
				} else if (j == e - 1){
					printf(" miss");
					//printf(" entering evict!");
					p->nummiss++;
					//printf(" miss");
					printf(" eviction");
					c_evict(ch, t, c[i].set, p);
					p->numevict++;
					//return 2; // Miss, Needs eviction + lru!!!!
					printf(" eviction");
					return;
				}
			}
		}
	}
	fprintf(stderr, "the cache has no such set indicated by the address!!!\n");
	exit(0);
}

int main(int argc, char **argv)
{
    param* p = malloc(sizeof(param));
	
	int rs, re, rb;
	
	FILE *read_trace;
	char* trace_file;
	char c;
    while( (c=getopt(argc,argv,"s:E:b:t:")) != -1)
	{
        switch(c)
		{
        case 's':
            rs = atoi(optarg);
            break;
        case 'E':
            re = atoi(optarg);
            break;
        case 'b':
            rb = atoi(optarg);
            break;
        case 't':
            trace_file = optarg;
            break;
        default:
            fprintf(stderr, "not enough arguments\n");
            exit(1);
        }
    }
	
	p->s = rs;
	p->b = rb;
	p->e = re;
	p->nummiss = 0;
	p->numhit = 0;
	p->numevict = 0;
	
	cache* ch = c_construct(p);
	read_trace = fopen(trace_file, "r");
	
	char cmd;
	unsigned long long address;
	int size;
	
	if (read_trace != NULL) {
		while (fscanf(read_trace, " %c %llx,%d", &cmd, &address, &size) == 3) {
			trace *t;
			//printf(" %c %llx,%d", cmd, address, size);
			switch(cmd) {
				case 'I':
					//free_trace(t);
					break;
				case 'L':
					printf(" %c %llx,%d", cmd, address, size);
					t = t_parse(address, p);
					c_search(ch, t, p);
					printf("\n");
					//free_trace(t);
					break;
				case 'S':
					printf(" %c %llx,%d", cmd, address, size);
					t = t_parse(address, p);
					c_search(ch, t, p);
					printf("\n");
					//free_trace(t);
					break;
				case 'M':
					printf(" %c %llx,%d", cmd, address, size);
					t = t_parse(address, p);
					c_search(ch, t, p);
					c_search(ch, t, p);
					printf("\n");
					//free_trace(t);
					break;
				default:
					//free_trace(t);
					break;
			}
		}
	}
	
	

	printSummary(p->numhit, p->nummiss, p->numevict);
	
	//free_cache(ch, p);

	//free(p);
	
    return 0;
}