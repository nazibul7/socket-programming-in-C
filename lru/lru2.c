#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TABLE_SIZE 10

typedef struct Node
{
    char *key;
    char *value;
    struct Node *hash_next;

    struct Node *prev;
    struct Node *next;
} Node;

typedef struct
{
    int capacity;
    int size;
    Node *head;
    Node *tail;
    Node *table[TABLE_SIZE];
} LRUCache;

Node *createNode(char *key, char *value)
{
    Node *node = malloc(sizeof(Node));
    node->key = key;
    node->value = value;
    node->prev = node->next = node->hash_next = NULL;
    return node;
}

int hashFunction(char *key)
{
    int index = 0;
    while (*key)
    {
        index = index + *key % TABLE_SIZE;
        key++;
    }
    return index % TABLE_SIZE;
}

LRUCache *lruCacaheCreate(int capacity)
{
    LRUCache *cache = malloc(sizeof(LRUCache));
    cache->head = NULL;
    cache->tail = NULL;
    cache->capacity = capacity;
    cache->size = 0;

    for (int i = 0; i < TABLE_SIZE; i++)
    {
        cache->table[i] = NULL;
    }
    return cache;
}

void removeFromList(LRUCache *cache, Node *node)
{
    Node *p = node->prev;
    Node *n = node->next;

    if (cache->head == node)
    {
        cache->head->next = node->next;
    }
    if (cache->tail == node)
    {
        cache->tail = p;
    }
    if (p)
    {
        p->next = n;
    }
    if (n)
    {
        n->prev = p;
    }
    node->next = node->prev = NULL;
}

void addToFront(LRUCache *cache, Node *node)
{
    node->next = cache->head;
    if (cache->head)
    {
        cache->head->prev = node;
    }
    else
    {
        cache->tail = node;
    }
    cache->head = node;
}

void removeFromHashTable(LRUCache *cache, char *key)
{
    int index = hashFunction(key);
    Node *current = cache->table[index];
    Node *prev = NULL;

    while (current)
    {
        if (strcmp(current->key, key) == 0)
        {
            if (prev)
            {
                prev->hash_next = current->hash_next;
            }
            else
            {
                cache->table[index] = current->hash_next;
            }
            return;
        }
        prev = current;
        current = current->hash_next;
    }
}

void lruCachePut(LRUCache *cache, char *key, char *value)
{
    int index = hashFunction(key);
    Node *current = cache->table[index];
    while (current)
    {
        if (strcmp(current->key, key) == 0)
        {
            current->value = value;
            removeFromList(cache, current);
            addToFront(cache, current);
            return;
        }
        current = current->hash_next;
    }
    if (cache->size == cache->capacity)
    {
        Node *removeNode = cache->tail;
        removeFromList(cache, removeNode);
        removeFromHashTable(cache, removeNode->key);
        cache->size--;
    }
    Node *newNode = createNode(key, value);
    newNode->hash_next = cache->table[index];
    cache->table[index] = newNode;
    addToFront(cache, newNode);
    cache->size++;
    return;
}

void lruCacheGet(LRUCache *cache, char *key)
{
    int index = hashFunction(key);
    Node *current = cache->table[index];
    while (current)
    {
        if (strcmp(current->key, key) == 0)
        {
            printf("Key %s value is %s\n", current->key, current->value);
            removeFromList(cache, current);
            addToFront(cache, current);
            return;
        }
        current = current->hash_next;
    }
    printf("Key %s is not on the list\n", key);
}

void lruCachePrintList(LRUCache *cache)
{
    Node *current = cache->head;
    while (current)
    {
        printf("[%s -> %s]", current->key, current->value);
        current = current->next;
    }
    printf("->NULL\n");
}

void lruCacheprintTable(LRUCache *cache)
{
    for (int i = 0; i < TABLE_SIZE; i++)
    {
        printf("Bucket[%d]", i);
        Node *current=cache->table[i];
        while (current)
        {
            printf(" [%s: %s] -> ", current->key, current->value);
            current=current->hash_next;
        }
        printf(" NULL\n");
    }
}

int main()
{
    LRUCache *cache = lruCacaheCreate(3);
    lruCachePut(cache, "dad", "rahaman");
    lruCachePut(cache, "mom", "doly");
    lruCachePut(cache, "me", "nazibul");
    
    lruCachePrintList(cache);
    lruCacheprintTable(cache);
    
    lruCacheGet(cache,"dad");
    
    lruCachePut(cache, "me", "nizam");
    lruCachePut(cache, "me1", "nazibul");
    lruCachePrintList(cache);
    lruCacheprintTable(cache);
    return 0;
}