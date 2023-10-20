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
BPlusTree *getParentPointer(const BPlusTree *node)
{
    return node->parent_pointer;
}

// Setter para parent_pointer
void setParentPointer(BPlusTree *node, BPlusTree *value)
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

int getNumKeys(const BPlusTree *node)
{
    return node->num_keys;
}
void initializeLeafNode(BPlusTree *node, bool is_root)
{
    setNodeType(node, LEAF_NODE);
    setIsRoot(node, is_root);
    setParentPointer(node, nullptr);
    setNumRecords(node, 0);
}

void setNumKeys(BPlusTree *node, int value)
{
    node->num_keys = value;
}

void setRightChildPtr(BPlusTree *node, BPlusTree *value)
{
    node->right_child_ptr = value;
}

void addKeyPtrPair(BPlusTree *node, std::pair<BPlusTree *, int> keyPtrPair)
{
    node->key_ptr_pairs.push_back(keyPtrPair);
    setNumKeys(node, getNumKeys(node) + 1);
}

BPlusTree *initializeInternalNode(BPlusTree *rightChildPtr, std::pair<BPlusTree *, int> keyPtrPair)
{
    BPlusTree *node = new BPlusTree;
    setNodeType(node, INTERNAL_NODE);
    setParentPointer(node, nullptr);
    setRightChildPtr(node, rightChildPtr);
    addKeyPtrPair(node, keyPtrPair);
    return node;
}

// Función para obtener todos los registros de un nodo hoja
std::vector<Record> getRecordsFromLeafNode(BPlusTree *node)
{

    std::vector<Record> records;
    for (const auto &entry : node->records)
    {
        records.push_back(deserializeRecord(entry.second));
    }

    return records;
}

void splitNode(BPlusTree *node, Record record)
{

    BPlusTree *newLeaf = new BPlusTree;
    BPlusTree *oldLeaf = new BPlusTree;
    initializeLeafNode(newLeaf, false);
    initializeLeafNode(oldLeaf, false);
    std::vector<Record> records = getRecordsFromLeavesNode(node);
    records.push_back(record);
    delete node;
    std::sort(records.begin(), records.end(), [](Record a, Record b)
              { return a.id < b.id; });
    int middle = (MAX_RECORDS + 1) / 2;
    BPlusTree *internal = initializeInternalNode(newLeaf, std::make_pair(oldLeaf, records[middle].id));
    internal->is_root = true;
    for (int i = 0; i < MAX_RECORDS + 1; i++)
    {
        if (i >= middle)
        {
            std::cout << "Inserting in new leaf" << std::endl;
            insertBPlus(newLeaf, records[i]);
        }
        else
        {
            std::cout << "Inserting in old leaf" << std::endl;
            insertBPlus(oldLeaf, records[i]);
        }
    }
    std::cout << "node type: " << (node->node_type ? "Internal" : "Leaf") << std::endl;
}

void decideInsertion(BPlusTree *node, Record record)
{
    if (node->node_type == INTERNAL_NODE)
    {
        for (auto &keyPtrPair : node->key_ptr_pairs)
        {
            if (getNumRecords(keyPtrPair.first) < MAX_RECORDS + 1 && record.id < keyPtrPair.second)
            {
                decideInsertion(keyPtrPair.first, record);
                break;
            }
            else
            {
                std::cout << "Split No implementado" << std::endl;
            }
        }
        decideInsertion(node->right_child_ptr, record);
    }
    else
    {
        std::cout << "Inserting in leaf" << record.id << record.email << record.user << std::endl;
        serializeRecord(record, node->records[record.id]);
        setNumRecords(node, getNumRecords(node) + 1);
    }
}

// Función para insertar un registro en un nodo hoja
void insertBPlus(BPlusTree *node, Record record)
{
    if (node->node_type == LEAF_NODE && getNumRecords(node) == MAX_RECORDS)
    {
        std::cout << "Splitting node" << std::endl;
        splitNode(node, record);
    }
    else
    {
        decideInsertion(node, record);
    }
};

std::vector<Record> getRecordsFromLeavesNode(BPlusTree *node)
{
    std::vector<Record> records;
    if (node->node_type != LEAF_NODE)
    {
        for (auto &keyPtrPair : node->key_ptr_pairs)
        {
            std::cout << "Records from internal node: " << std::endl;

            std::vector<Record> recordsFromChild = getRecordsFromLeafNode(keyPtrPair.first);
            records.insert(records.end(), recordsFromChild.begin(), recordsFromChild.end());
        }
        std::vector<Record> recordsFromRightChild = getRecordsFromLeafNode(node->right_child_ptr);
        std::cout << "Records from right child: " << recordsFromRightChild.size() << std::endl;
        records.insert(records.end(), recordsFromRightChild.begin(), recordsFromRightChild.end());

        return records;
    }
    else
    {
        std::cout << "Records from leaf node: " << std::endl;
        return getRecordsFromLeafNode(node);
    }
}
