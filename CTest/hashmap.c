#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
typedef struct HashNode{
    int key;
    int value;
    struct HashNode *next;
} HashNode;

typedef struct HashTable{
    HashNode **buckets;
    size_t size;
    size_t count;
}HashTable; 

HashTable *create_hash_table(size_t size){ 
    HashTable *table = malloc(sizeof(HashTable));
    table->size = size;
    table->count = 0;
    table->buckets = calloc(size, sizeof(HashNode *));
    return table;
}

size_t hash_function(int key, size_t size){
    // 常用做法：key % table_size
    // 但为了避免冲突集中，可以用更好的散列方式
    return (size_t)(key * 31 + 17) % size;  // 简单乘法哈希
}

bool hash_table_put(HashTable *table, int key, int value){ 
    size_t index = hash_function(key, table->size);
    HashNode *node = table->buckets[index];
    while(node){
        if(node->key == key){
            node->value = value;
            return true;
        }
        node = node->next;
    }

    HashNode* newNode = malloc(sizeof(HashNode));
    newNode->key = key;
    newNode->value = value;
    newNode->next = table->buckets[index];
    table->buckets[index] = newNode;

    table->count++;
    return true;
}

int hash_table_get(HashTable *table, int key, bool *found){ 
    size_t index = hash_function(key, table->size);
    HashNode *node = table->buckets[index];
    while(node){
        if(node->key == key){
            *found = true;
            return node->value;
        }
        node = node->next;
    }
    *found = false;
    return -1;
}

bool hash_table_remove(HashTable *table, int key){ 
    size_t index = hash_function(key, table->size);
    HashNode *node = table->buckets[index];
    HashNode *prev = NULL;
    while(node){
        if(node->key == key){
            if(prev){
                prev->next = node->next;
            }else{
                table->buckets[index] = node->next;
            }
            free(node);
            table->count--;
            return true;
        }

        prev = node;
        node = node->next;
    }

    return false;
}

static bool resize_hash_table(HashTable *table){
    size_t new_size = table->size * 2;
    HashTable **new_buckets = calloc(new_size, sizeof(HashNode *));
    for(size_t i = 0; i < table->size; i++){ 
        HashNode *node = table->buckets[i];
        while(node){
            size_t new_index = hash_function(node->key, new_size);
            HashNode *next = node->next;
            node->next = new_buckets[new_index];
            new_buckets[new_index] = node;
            node = next;
        }
    }

    free(table->buckets);
    table->buckets = new_buckets;
    table->size = new_size;
    return true;
}