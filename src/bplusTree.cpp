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
    int middle = (MAX_RECORDS + 1) / 2;
    std::vector<Record> records = getRecordsFromLeafNode(node);
    std::sort(records.begin(), records.end(), [](Record a, Record b)
              { return a.id < b.id; });
    records.push_back(record);
    for (auto it = std::next(records.begin(), middle); it != records.end(); it++)
    {
        node->records.erase(it->id);
        node->num_records--;
        insertBPlus(newLeaf, *it);
    }
    node->num_records++;
    BPlusTree internal = createNode(INTERNAL_NODE);
    node->parent_pointer = internal;
    newLeaf->parent_pointer = internal;
    internal->key_ptr_pairs.push_back(std::make_pair(node, records[middle - 1].id));
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

    int middle = (MAX_RECORDS + 1) / 2;
    for (auto it = std::next(records.begin(), middle); it != records.end(); it++)
    {

        node->records.erase(it->id);
        node->num_records--;
        insertBPlus(newLeaf, *it);
    }
    if (record.id < records[middle - 1].id)
    {
        insertBPlus(node, record);
    }
    else
    {
        insertBPlus(newLeaf, record);
    }
    BPlusTree parent = node->parent_pointer;
    newLeaf->parent_pointer = parent;
    if (parent->key_ptr_pairs[parent->num_keys - 1].second < record.id)
    {
        parent->key_ptr_pairs.push_back(std::make_pair(parent->right_child_ptr, records[middle - 1].id));
        parent->right_child_ptr = newLeaf;
    }
    else
    {
        int last_key = parent->key_ptr_pairs[parent->num_keys - 1].second;
        parent->key_ptr_pairs[parent->num_keys - 1].first = node;
        parent->key_ptr_pairs[parent->num_keys - 1].second = records[middle - 1].id;
        parent->key_ptr_pairs.push_back(std::make_pair(newLeaf, last_key));
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

bool isFullLeaf(BPlusTree &node)
{
    return node->num_records >= MAX_RECORDS;
}

void splitInternalNode(BPlusTree &node, Record record)
{
    BPlusTree newInternal = createNode(INTERNAL_NODE);
    std::vector<std::pair<BPlusTree, int>> keyPtrPairs = node->key_ptr_pairs;
    int middle = (NODE_SIZE + 1) / 2;
    for (auto it = std::next(keyPtrPairs.begin(), middle); it != keyPtrPairs.end(); it++)
    {
        node->num_keys--;
        newInternal->num_keys++;
        newInternal->key_ptr_pairs.push_back(*it);
    }
    if (record.id < keyPtrPairs[middle - 1].second)
    {
        insertBPlus(node, record);
    }
    else
    {
        insertBPlus(newInternal, record);
    }
    BPlusTree parent = node->parent_pointer;
    newInternal->parent_pointer = parent;
}

void decideInsertion(BPlusTree &node, Record record)
{
    if (node->node_type == INTERNAL_NODE)
    {
        if (node->num_keys >= NODE_SIZE)
        {
            splitInternalNode(node, record);
            std::cout << "Nodo interno lleno, split no implementado" << std::endl;
            return;
        }

        for (auto &keyPtrPair : node->key_ptr_pairs)
        {
            if (record.id < keyPtrPair.second)
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

void printBPlusTree(BPlusTree &node)
{
    if (node->node_type == LEAF_NODE)
    {
        std::cout << "Leaf node" << std::endl;
        std::cout << "Num records: " << node->num_records << std::endl;
        std::cout << "Records: " << std::endl;
        for (auto &record : getRecordsFromLeafNode(node))
        {
            std::cout << record.id << " " << record.email << std::endl;
        }
    }
    else
    {
        std::cout << "Internal node" << std::endl;
        std::cout << "Num keys: " << node->num_keys << std::endl;
        std::cout << "Keys: " << std::endl;
        for (auto &keyPtrPair : node->key_ptr_pairs)
        {
            std::cout << keyPtrPair.second << std::endl;
        }
        for (auto &keyPtrPair : node->key_ptr_pairs)
        {
            std::cout << "Left child: " << std::endl;
            printBPlusTree(keyPtrPair.first);
        }
        std::cout << "Right child: " << std::endl;
        printBPlusTree(node->right_child_ptr);
    }
}