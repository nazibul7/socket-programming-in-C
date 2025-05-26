#include <stdio.h>

#define TABLE_SIZE 10
#define EMPTY -1
typedef struct
{
    int key;
    int value;
} HashType;

HashType HashTable[TABLE_SIZE];

int hashFunction(int key)
{
    return key % TABLE_SIZE;
}

void initTable()
{
    for (int i = 0; i < TABLE_SIZE; i++)
    {
        HashTable[i].key = EMPTY;
    }
}

void insert(int key, int value)
{
    int index = hashFunction(key);
    int startIndex = index;
    while (HashTable[index].key != EMPTY)
    {
        if (HashTable[index].key == key)
        {
            HashTable[index].value = value;
            return;
        }

        index = (index + 1) % TABLE_SIZE;
        if (startIndex == index)
        {
            printf("Hash Table is full\n");
            return;
        }
    }
    HashTable[index].key = key;
    HashTable[index].value = value;
}

int search(int key)
{
    int index = hashFunction(key);
    int startIndex = index;
    while (HashTable[index].key != EMPTY)
    {
        if (HashTable[index].key == key)
        {
            return HashTable[index].value;
        }
        else
        {
            index = (index + 1) % TABLE_SIZE;
        }
        if (startIndex == index)
        {
            printf("Value of the corresponding key from HashTable is not found");
            return -1;
        }
    }
    return -1;
}

void display()
{
    int i = 0;
    while (i < TABLE_SIZE)
    {
        if (HashTable[i].key != EMPTY)
        {
            printf("Index %d: Key = %d, Value = %d\n", i, HashTable[i].key, HashTable[i].value);
        }
        else
        {
            printf("Index %d: Empty\n", i);
        }
        i++;
    }
}

void delete(int key)
{
    int index = hashFunction(key);
    while (HashTable[index].key == key)
    {
        HashTable[index].key = EMPTY;
        HashTable[index].value = EMPTY;
    }
}

int main()
{
    initTable();
    insert(11, 26);
    insert(2, 24);
    insert(12, 48);
    insert(1, 55);

    insert(1, 10);
    insert(11, 20);
    insert(21, 30);

    display();
    printf("Search key with value: %d\n", search(11));
    delete (2);
    printf("After deletion key & value pair is\n");
    display();
    printf("Search key with value: %d\n", search(2));
    return 0;
}