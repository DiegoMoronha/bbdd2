#define MAX_RECORDS 4
#define KEY_SIZE 4
#define VALUE_SIZE 291
#define NODE_SIZE 2
#define LEAF_NODE 0
#define INTERNAL_NODE 1

#include <string.h>
#include <iostream>
#include <math.h>
#include <vector>
#include "record.h"
#include <map>
#include <algorithm>

struct BPlusTreeST
{
    unsigned char node_type;     // 1 byte
    bool is_root;                // 1 byte
    BPlusTreeST *parent_pointer; // 4 bytes
    int num_records;             // 4 bytes
    std::map<int, char[VALUE_SIZE]> records;
    // nodo interno exclusivo
    int num_keys;                                             // 4 bytes
    BPlusTreeST *right_child_ptr;                             // 4 bytes
    std::vector<std::pair<BPlusTreeST *, int>> key_ptr_pairs; // Hasta 510 pares puntero-clave

    ~BPlusTreeST()
    {
        delete parent_pointer;
        delete right_child_ptr;
        if (node_type == INTERNAL_NODE)
        {
            for (auto pair : key_ptr_pairs)
            {
                delete pair.first;
            }
        }
    }
};

typedef struct BPlusTreeST *BPlusTree;

std::vector<Record> getRecords(BPlusTree node);
void insertBPlus(BPlusTree &node, Record record);
int countNumRecords(BPlusTree node);
void destroyBPlusTree(BPlusTree &node);
void printBPlusTree(BPlusTree node);