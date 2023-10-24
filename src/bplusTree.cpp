#include "bplusTree.h"

char getNodeType(const BPlusTree node)
{
    return node->node_type;
}

void setNodeType(BPlusTree node, char value)
{
    node->node_type = value;
}

char getIsRoot(const BPlusTree node)
{
    return node->is_root;
}

void setIsRoot(BPlusTree node, char value)
{
    node->is_root = value;
}

// Getter para parent_pointer
BPlusTree getParentPointer(const BPlusTree node)
{
    return node->parent_pointer;
}

// Setter para parent_pointer
void setParentPointer(BPlusTree node, BPlusTree value)
{
    node->parent_pointer = value;
}

int getNumRecords(const BPlusTree node)
{
    return node->num_records;
}

void setNumRecords(BPlusTree node, int value)
{
    node->num_records = value;
}

int getNumKeys(const BPlusTree node)
{
    return node->num_keys;
}

void setNumKeys(BPlusTree node, int value)
{
    node->num_keys = value;
}

void setRightChildPtr(BPlusTree node, BPlusTree value)
{
    node->right_child_ptr = value;
}

std::vector<Record> getRecordsFromLeafNode(BPlusTree node)
{
    std::vector<Record> records;
    for (const auto &entry : node->records)
    {
        records.push_back(deserializeRecord(entry.second));
    }

    return records;
}

BPlusTree createNode(unsigned char node_type)
{
    BPlusTree node = new BPlusTreeST();
    node->node_type = node_type;
    node->is_root = false;
    node->parent_pointer = nullptr;
    node->num_records = 0;
    node->num_keys = 0;
    node->right_child_ptr = nullptr;
    node->key_ptr_pairs.clear();
    node->records.clear();
    return node;
}

void splitLeafNodeRoot(BPlusTree &node, Record record)
{
    node->is_root = false;
    BPlusTree newLeaf = createNode(LEAF_NODE);
    int middle = MAX_RECORDS / 2;
    std::vector<Record> records = getRecordsFromLeafNode(node);
    std::sort(records.begin(), records.end(), [](Record a, Record b)
              { return a.id < b.id; });
    for (auto it = std::next(records.begin(), middle); it != records.end(); it++)
    {
        node->records.erase(it->id);
        node->num_records--;
        insertBPlus(newLeaf, *it);
    }
    insertBPlus(newLeaf, record);
    BPlusTree internal = createNode(INTERNAL_NODE);
    node->parent_pointer = internal;
    newLeaf->parent_pointer = internal;
    internal->key_ptr_pairs.push_back(std::make_pair(node, records[middle].id));
    internal->num_keys++;
    std::sort(internal->key_ptr_pairs.begin(), internal->key_ptr_pairs.end(), [](std::pair<BPlusTree, int> a, std::pair<BPlusTree, int> b)
              { return a.second < b.second; });
    internal->right_child_ptr = newLeaf;
    internal->is_root = true;

    node = internal;
}

void splitLeafNode(BPlusTree &node, Record record)
{
    BPlusTree newLeaf = createNode(LEAF_NODE);
    std::vector<Record> records = getRecordsFromLeafNode(node);

    std::sort(records.begin(), records.end(), [](Record a, Record b)
              { return a.id < b.id; });
    int middle = MAX_RECORDS / 2;
    for (auto it = std::next(records.begin(), middle); it != records.end(); it++)
    {
        node->records.erase(it->id);
        node->num_records--;
        insertBPlus(newLeaf, *it);
    }
    insertBPlus(newLeaf, record);
    BPlusTree parent = node->parent_pointer;
    newLeaf->parent_pointer = parent;
    if (parent->key_ptr_pairs[parent->num_keys - 1].second < record.id)
    {
        parent->key_ptr_pairs.push_back(std::make_pair(parent->right_child_ptr, parent->key_ptr_pairs[parent->num_keys - 1].second));
        parent->right_child_ptr = newLeaf;
    }
    else
    {
        parent->key_ptr_pairs.push_back(std::make_pair(newLeaf, records[middle].id));
    }
    std::sort(parent->key_ptr_pairs.begin(), parent->key_ptr_pairs.end(), [](std::pair<BPlusTree, int> a, std::pair<BPlusTree, int> b)
              { return a.second < b.second; });
    parent->num_keys++;
}
void splitNode(BPlusTree &node, Record record)
{
    if (node->is_root)
    {
        splitLeafNodeRoot(node, record);
    }
    else
    {
        splitLeafNode(node, record);
    }
}

bool canInsert(BPlusTree &node, std::pair<BPlusTree, int> keyPtrPair, Record record)
{
    return keyPtrPair.first->num_records < MAX_RECORDS && record.id < keyPtrPair.second;
}

bool isFullLeaf(BPlusTree &node)
{
    return node->num_records >= MAX_RECORDS;
}

void decideInsertion(BPlusTree &node, Record record)
{
    if (node->node_type == INTERNAL_NODE)
    {

        for (auto &keyPtrPair : node->key_ptr_pairs)
        {
            if (canInsert(node, keyPtrPair, record))
            {
                insertBPlus(keyPtrPair.first, record);
                return;
            }
        }

        insertBPlus(node->right_child_ptr, record);
    }
    else
    {
        serializeRecord(record, node->records[record.id]);
        node->num_records++;
    }
}

void initializeNodeIsNull(BPlusTree &node)
{
    if (node == nullptr)
    {
        node = createNode(LEAF_NODE);
        node->is_root = true;
    }
}

// Función para insertar un registro en un nodo hoja
void insertBPlus(BPlusTree &node, Record record)
{
    initializeNodeIsNull(node);
    if (node->node_type == LEAF_NODE && isFullLeaf(node))
    {
        splitNode(node, record);
    }
    else
    {
        decideInsertion(node, record);
    }
};

std::vector<Record> getRecordsFromLeavesNode(BPlusTree node)
{
    if (node == nullptr)
    {
        return std::vector<Record>();
    }

    std::vector<Record> records;
    if (node->node_type != LEAF_NODE)
    {
        for (auto &keyPtrPair : node->key_ptr_pairs)
        {
            std::vector<Record> recordsFromChild = getRecordsFromLeafNode(keyPtrPair.first);
            std::cout << "vectors count: " << recordsFromChild.size() << std::endl;
            records.insert(records.end(), recordsFromChild.begin(), recordsFromChild.end());
        }
        std::vector<Record> recordsFromRightChild = getRecordsFromLeafNode(node->right_child_ptr);
        records.insert(records.end(), recordsFromRightChild.begin(), recordsFromRightChild.end());

        return records;
    }
    else
    {
        return getRecordsFromLeafNode(node);
    }
}

int countNumRecords(BPlusTree node)
{
    if (node->node_type == LEAF_NODE)
    {
        return node->num_records;
    }
    else
    {
        int numRecords = 0;
        for (auto &keyPtrPair : node->key_ptr_pairs)
        {
            numRecords += countNumRecords(keyPtrPair.first);
        }
        numRecords += countNumRecords(node->right_child_ptr);
        return numRecords;
    }
}

void destroyBPlusTree(BPlusTree node)
{
    if (node->node_type == LEAF_NODE)
    {
        delete node;
    }
    else
    {
        for (auto &keyPtrPair : node->key_ptr_pairs)
        {
            destroyBPlusTree(keyPtrPair.first);
        }
        destroyBPlusTree(node->right_child_ptr);
        delete node;
    }
}