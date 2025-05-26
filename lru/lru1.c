#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Node
{
    char *key;
    char *value;
} Node;

typedef struct LRUCache
{
    Node *arr;
    int capacity;
    int size;
} LRUCache;

int indexKey(char *key, LRUCache *cache)
{
    int index;
    for (int i = 0; i < cache->size; i++)
    {
        if (strcmp(cache->arr[i].key, key) == 0)
        {
            index = i;
            return index;
        }
    }
    index = -1;
    return index;
}

LRUCache *lruCacheCreate(int capacity)
{
    LRUCache *cache = malloc(sizeof(LRUCache));
    cache->capacity = capacity;
    cache->size = 0;
    cache->arr = malloc(sizeof(Node) * capacity);
    return cache;
}

void changeOrder(LRUCache *cache,char *key){
    int index=indexKey(key,cache);
    if(index==0) return;
    Node temp=cache->arr[index];
    for(int i=index;i>0;i--){
        cache->arr[i]=cache->arr[i-1];
    }
    cache->arr[0]=temp;
    return;
}

void lruCachePut(LRUCache *cache, char *key, char *value)
{
    int index = indexKey(key, cache);
    if (index != -1)
    {
        cache->arr[index].value = value;
        changeOrder(cache,key);
        return;
    }
    else
    {
        if (cache->size < cache->capacity)
        {
            for (int i = cache->size; i > 0; i--)
            {
                cache->arr[i] = cache->arr[i - 1];
            }
            cache->arr[0].key = strdup(key);
            cache->arr[0].value = strdup(value);
            cache->size++;
        }
        else
        {
            for (int i = cache->capacity - 1; i > 0; i--)
            {
                cache->arr[i]=cache->arr[i-1];
            }
            cache->arr[0].key = strdup(key);
            cache->arr[0].value = strdup(value);
        }
    }
}

void getLruCache(LRUCache *cache,char *key){
    int index=indexKey(key,cache);

    if(index!=-1){
        if(strcmp(cache->arr[index].key,key)==0){
            printf("Key of %s value is %s\n",key,cache->arr[index].value);
            changeOrder(cache,key);
            return;
        }
    }
    else{
        printf("Key %s not found\n",key);
        return;
    }
}

void printLru(LRUCache *cache){
    for(int i=0;i<cache->size;i++){
        printf("[%s -> %s]---",cache->arr[i].key,cache->arr[i].value);
    }
    printf("\n");
}

int main()
{
    LRUCache *cache=lruCacheCreate(5);

    lruCachePut(cache,"father","rahaman");
    lruCachePut(cache,"mother","doly");
    lruCachePut(cache,"me","nazibul");
    lruCachePut(cache,"bro","nizam");
    lruCachePut(cache,"school","BKSI");

    printLru(cache);
    
    getLruCache(cache,"bro");
    
    printLru(cache);
    return 0;
}