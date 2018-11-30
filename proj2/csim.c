/***********************************************************************
 * csim.c 
 * 
 * Name: Yining Liu
 * ID: 516030910125
 ***********************************************************************/

#include "cachelab.h"
#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>


typedef struct{
    int valid;
    int tag;
    int LRU;
}Line;

typedef struct{
    Line* lines;
}Set;

typedef struct {
    int setNum;
    int lineNum;
    Set* sets;
}Cache;

Cache cache;
int hits,misses,evictions;

int findMin(int set);
void initCache(int s,int E,int b);
void sim(int addr,int size,int set,int tag ,int verbose);

int main(int argc, char** argv){
    int s,E,b,verbose=0;
    int addr,size;    
    int set,tag;
    char opt;
    char *tracefile;

    //read-in parameters
    while((opt = getopt(argc,argv,"vs:E:b:t:"))!=-1){
        switch(opt){
            case 'v': verbose = 1; break;
            case 's': s = atoi(optarg); break;
            case 'E': E = atoi(optarg); break;
            case 'b': b = atoi(optarg); break;
            case 't': tracefile = optarg;
        }
    }

    //init
    initCache(s,E,b);

    //operating
    freopen(tracefile,"r",stdin);  //open tracefile
    while(scanf("%c %x,%d",&opt,&addr,&size) != EOF){
                if(opt=='L'||opt=='S'||opt=='M'){  //ignore "I"
                set = (addr>>b)&((1<<s)-1);  //get set index
	        tag = addr>>(s+b);           //get tag

                if(verbose==1) printf("%c %x,%d ",opt,addr,size);  //%x for hex
                if(opt=='L'||opt=='S') sim(addr,size,set,tag,verbose);//load or store
                else if(opt=='M') {
                    sim(addr,size,set,tag,verbose);    //load
                    sim(addr,size,set,tag,verbose);    //store
                }
                if(verbose==1) printf("\n");
           }
    }

    //print result
    printSummary(hits,misses,evictions);

    return 0;
}

void sim(int addr,int size,int set,int tag ,int verbose){
    int i,j;
    int miss = 1,full = 1;

    //miss or not
    for(i=0;i<cache.lineNum;i++){
        if(cache.sets[set].lines[i].valid==1&&cache.sets[set].lines[i].tag==tag){  //hit!
            miss = 0;
            //update LRU
            cache.sets[set].lines[i].LRU = 2333;
            for(j=0;j<cache.lineNum;j++){
                if(j!=i) cache.sets[set].lines[j].LRU--;
            }
        }
    }

    if(!miss){
        hits++;
        if(verbose == 1) printf("hit ");
    }
    else{
        misses++;
        if(verbose == 1) printf("miss ");

        //this set: full or not
        for(i=0;i<cache.lineNum;i++){
            if(cache.sets[set].lines[i].valid==0){
                full = 0;
                break;
            }
        }
        if(full){
            i = findMin(set); ////eviction happens
            evictions++;
            if(verbose == 1) printf("eviction ");
        }
        cache.sets[set].lines[i].valid = 1;
        cache.sets[set].lines[i].tag = tag;
        cache.sets[set].lines[i].LRU = 2333;
        for(j=0;j<cache.lineNum;j++){
            if(j!=i) cache.sets[set].lines[j].LRU--;
        }
    }

}

// find new minimum LRU
int findMin(int set){
    int i;
    int minIdx = 0, minLRU=2333;

    for(i=0;i<cache.lineNum;i++){
        if(cache.sets[set].lines[i].LRU<minLRU){
            minIdx = i;
            minLRU = cache.sets[set].lines[i].LRU;
        }
    }
    
    return minIdx;
}

void initCache(int s,int E,int b){
    int i ,j;

    cache.setNum = 2 << s;
    cache.lineNum = E;
    cache.sets = (Set *)malloc(cache.setNum * sizeof(Set));

    for(i=0;i<cache.setNum;i++){
        cache.sets[i].lines = (Line *)malloc(E*sizeof(Line));
        for(j=0;j<E;j++){
            cache.sets[i].lines[j].valid = 0;
            cache.sets[i].lines[j].LRU = 0;
        }
    }
}



