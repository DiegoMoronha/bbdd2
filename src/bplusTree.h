#define MAX_RECORDS 14
#define KEY_SIZE 4
#define VALUE_SIZE 291
#define NODE_SIZE 511
#define LEAF_NODE 0
#define INTERNAL_NODE 1

#include <string.h>
#include <iostream>
#include <math.h>
#include <vector>
#include "record.h"
#include <map>
#include <algorithm>

struct BPlusNode
{
    unsigned char node_type;   // 1 byte
    bool is_root;              // 1 byte
    BPlusNode *parent_pointer; // 4 bytes
    int num_records;           // 4 bytes
    std::pair<int, char[VALUE_SIZE]> **records;
    // nodo interno exclusivo
    int num_keys;                                // 4 bytes
    BPlusNode *right_child_ptr;                  // 4 bytes
    std::pair<BPlusNode *, int> **key_ptr_pairs; // Hasta 510 pares puntero-clave

    BPlusNode(unsigned char node_type, int max_records = MAX_RECORDS, int max_keys = NODE_SIZE)
    {
        this->node_type = node_type;
        this->is_root = false;
        this->parent_pointer = nullptr;
        this->right_child_ptr = nullptr;
        this->records = new std::pair<int, char[VALUE_SIZE]> *[max_records];
        this->key_ptr_pairs = new std::pair<BPlusNode *, int> *[max_keys];
        this->num_keys = 0;
        this->num_records = 0;
    };

    ~BPlusNode()
    {
        if (this->node_type == LEAF_NODE)
        {
            for (int i = 0; i < num_records; i++)
            {
                delete records[i];
            }
            delete[] records;
        }
        else
        {
            for (int i = 0; i < num_keys; i++)
            {
                delete key_ptr_pairs[i]->first;
            }
            delete[] key_ptr_pairs;
        }
    };
};

struct BPlusTree
{
    BPlusNode *root;
    int totalRecords;
    int totalNodes;
    int internalSize;
    int leafSize;

    BPlusTree(int internalSize = NODE_SIZE, int leafSize = MAX_RECORDS)
    {
        this->root = nullptr;
        this->internalSize = internalSize;
        this->leafSize = leafSize;
        this->totalRecords = 0;
        this->totalNodes = 0;
    };

public:
    void Insert(Record record);
    std::vector<Record> GetRecords();
};

//