#include "bplusTree.h"

char getNodeType(const BPlusTree *node)
{
    return node->node_type;
}

void setNodeType(BPlusTree *node, char value)
{
    node->node_type = value;
}

char getIsRoot(const BPlusTree *node)
{
    return node->is_root;
}

void setIsRoot(BPlusTree *node, char value)
{
    node->is_root = value;
}

// Getter para parent_pointer
int getParentPointer(const BPlusTree *node)
{
    return node->parent_pointer;
}

// Setter para parent_pointer
void setParentPointer(BPlusTree *node, int value)
{
    node->parent_pointer = value;
}

int getNumRecords(const BPlusTree *node)
{
    return node->num_records;
}

void setNumRecords(BPlusTree *node, int value)
{
    node->num_records = value;
}

void initializeLeafNode(BPlusTree *node)
{
    setNodeType(node, 1);
    setIsRoot(node, 0);
    setParentPointer(node, -1);
    setNumRecords(node, 0);
}

// Función para insertar un registro en un nodo hoja
void insertIntoLeafNode(BPlusTree *node, Record record)
{
    serializeRecord(record, node->records[node->num_records]);
    setNumRecords(node, getNumRecords(node) + 1);
};

std::vector<Record> getRecordsFromLeafNode(BPlusTree *node)
{
    std::vector<Record> records;
    for (int i = 0; i < getNumRecords(node); i++)
    {
        records.push_back(deserializeRecord(node->records[i]));
    }
    return records;
}