#define MAX_RECORDS 13
#define KEY_SIZE 4
#define VALUE_SIZE 291
#define NODE_SIZE 512
#define LEAF_NODE 0
#define INTERNAL_NODE 1

#include <string.h>
#include <iostream>
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
};

typedef struct BPlusTreeST *BPlusTree;

std::vector<Record> getRecordsFromLeavesNode(BPlusTree node);
void insertBPlus(BPlusTree &node, Record record);
int countNumRecords(BPlusTree node);
void destroyBPlusTree(BPlusTree node);