#define MAX_RECORDS 13
#define KEY_SIZE 4
#define VALUE_SIZE 291

#include <string.h>
#include <iostream>
#include <vector>
#include "record.h"

struct BPlusTree
{
    unsigned char node_type; // 1 byte
    unsigned char is_root;   // 1 byte
    int parent_pointer;      // 4 bytes
    int num_records;         // 4 bytes
    char records[MAX_RECORDS][VALUE_SIZE];
};

std::vector<Record> getRecordsFromLeafNode(BPlusTree *node);
void insertIntoLeafNode(BPlusTree *node, Record record);
void initializeLeafNode(BPlusTree *node);
