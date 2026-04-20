#include<stdio.h>
#include<stdlib.h>
#include "cachelab.h"
#include<getopt.h>
#include<unistd.h>

typedef enum{True = 1,False = 0} bool;
typedef struct {
    bool valid;  //valid bit
    unsigned long Tag; //tag bit
    int LRUCounter; //count recent use
}CacheLine;

typedef struct {
    unsigned long E; //the number of CacheLine
    CacheLine* pCacheLine;
}CacheSet;

typedef struct{
    unsigned long S;  //the number of CacheSet
    CacheSet* pCacheSet;
}Cache;

Cache* InitCache(int s,int E,int b){
    Cache* pCache = (Cache*)malloc(sizeof(Cache));
    int S = 1 << s;
    pCache->S = S;
    pCache->pCacheSet = (CacheSet*)malloc(pCache->S * sizeof(CacheSet));
    CacheSet* pCacheSetIdx = pCache->pCacheSet;
    for(int i = 0;i < pCache->S;i++){
        pCacheSetIdx->E = E;
        pCacheSetIdx->pCacheLine = (CacheLine*)malloc(E*sizeof(CacheLine));
        CacheLine* pCacheLineIdx = pCacheSetIdx->pCacheLine;
        for(int j = 0;j < pCache->pCacheSet->E;j++){
            pCacheLineIdx->valid = False;
            pCacheLineIdx->Tag = 0;
            pCacheLineIdx->LRUCounter = 0;
            pCacheLineIdx++;
        }
        pCacheSetIdx++;
    }
    return pCache;
}


void FreeCache(Cache* pCache){
    CacheSet* pCacheSetIdx = pCache->pCacheSet;
    for(int i = 0;i < pCache->S;i++){
        free(pCacheSetIdx->pCacheLine);
        pCacheSetIdx++;
    }
    free(pCache->pCacheSet);
    free(pCache);
}


void accessCache(Cache* pCache,unsigned long *setIndex,unsigned long *tag,int *globalTimer,int *hitCount,int *missCount,int *evictionCount){
    (*globalTimer)++;
    CacheSet* targetSet = &(pCache->pCacheSet[*setIndex]);
    for(int i = 0;i < targetSet->E;i++){
        if(targetSet->pCacheLine[i].valid == True && targetSet->pCacheLine[i].Tag == *tag){
            (*hitCount)++;
            targetSet->pCacheLine[i].LRUCounter = (*globalTimer);
            return;
        }
    }
    (*missCount)++;
    for(int j = 0;j < targetSet->E;j++){
        if(targetSet->pCacheLine[j].valid == False){
            targetSet->pCacheLine[j].valid = True;
            targetSet->pCacheLine[j].Tag = *(tag);
            targetSet->pCacheLine[j].LRUCounter = (*globalTimer);
            return;
        } 
    }
    (*evictionCount)++;
    int minLRU = targetSet->pCacheLine[0].LRUCounter;
    int minLRUw = 0;
    for(int w = 0;w < targetSet->E;w++){
        if(targetSet->pCacheLine[w].LRUCounter < minLRU){
            minLRU = targetSet->pCacheLine[w].LRUCounter;
            minLRUw = w;
        }
    }
    targetSet->pCacheLine[minLRUw].Tag = *(tag);
    targetSet->pCacheLine[minLRUw].LRUCounter = (*globalTimer);
    return;
}



int main(int argc,char** argv){
    int s,E,b;
    char opt;
    char* trace_file;
    int hitCount = 0;//The time of Cache Hit
    int missCount = 0;//the time of Cache Miss
    int evictionCount = 0;//
    int globalTimer = 0;//simulate the passing time
    while((opt = getopt(argc,argv,"s:E:b:t:v")) != -1){
        switch (opt)
        {
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
            trace_file = optarg;
            break;
        case 'v':
            break;
        default:
            break;
        }
    }
    
    FILE* pFile = fopen(trace_file,"r");
    if(pFile == NULL){
        printf("Fail to open the file!");
        return 0;
    }
    char operation;
    unsigned long address;
    //address [Tag][SetIdx(s)][Block Offset(b)]
    int size;
    Cache* pCache = InitCache(s,E,b);
    while(fscanf(pFile," %c %lx,%d",&operation,&address,&size) == 3){
        if(operation == 'I') continue;
        unsigned long setIndex = (address >> b) & ((1ULL << s) - 1);
        unsigned long tag = address >> (b+s);
        if(operation == 'L' || operation == 'S'){
            accessCache(pCache,&setIndex,&tag,&globalTimer,&hitCount,&missCount,&evictionCount);
        }
        else if(operation == 'M'){
            accessCache(pCache,&setIndex,&tag,&globalTimer,&hitCount,&missCount,&evictionCount);
            accessCache(pCache,&setIndex,&tag,&globalTimer,&hitCount,&missCount,&evictionCount);
        }
    }
    fclose(pFile);
    FreeCache(pCache);
    printSummary(hitCount,missCount,evictionCount);
    return 0;
}