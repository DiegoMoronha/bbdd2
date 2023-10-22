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

void addKeyPtrPair(BPlusTree node, std::pair<BPlusTree, int> keyPtrPair)
{
    node->key_ptr_pairs.push_back(keyPtrPair);
    setNumKeys(node, getNumKeys(node) + 1);
}

// Función para obtener todos los registros de un nodo hoja
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

void splitNode(BPlusTree &node, Record record)
{
    node->is_root = false;
    BPlusTree newLeaf = createNode(LEAF_NODE);
    std::vector<Record> records = getRecordsFromLeavesNode(node);
    records.push_back(record);
    std::sort(records.begin(), records.end(), [](Record a, Record b)
              { return a.id < b.id; });
    int middle = (MAX_RECORDS + 1) / 2;
    for (auto it = std::next(records.begin(), middle); it != records.end(); it++)
    {
        node->records.erase(it->id);
        node->num_records--;

        insertBPlus(newLeaf, *it);
    }
    BPlusTree internal = createNode(INTERNAL_NODE);
    internal->key_ptr_pairs.push_back(std::make_pair(node, records[middle].id));
    std::sort(internal->key_ptr_pairs.begin(), internal->key_ptr_pairs.end(), [](std::pair<BPlusTree, int> a, std::pair<BPlusTree, int> b)
              { return a.second < b.second; });
    internal->right_child_ptr = newLeaf;
    internal->is_root = true;
    node = internal;
    std::cout << "Is root:|" << (node->is_root ? "True" : "False") << "|" << std::endl;
    std::cout << "Node type:|" << (node->node_type ? "Internal" : "Leaf") << "|" << std::endl;
}

void decideInsertion(BPlusTree &node, Record record)
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
        node->num_records++;
    }
}

void initializeNodeIsNull(BPlusTree &node)
{
    if (node == nullptr)
    {
        std::cout << "Creating root node" << std::endl;
        node = createNode(LEAF_NODE);
        node->is_root = true;
    }
}

// Función para insertar un registro en un nodo hoja
void insertBPlus(BPlusTree &node, Record record)
{
    initializeNodeIsNull(node);
    if (node->node_type == LEAF_NODE && node->num_records >= MAX_RECORDS)
    {
        std::cout << "Splitting node" << std::endl;
        splitNode(node, record);
    }
    else
    {
        decideInsertion(node, record);
    }
};

std::vector<Record> getRecordsFromLeavesNode(BPlusTree node)
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
        std::cout << "Records from leaf node " << std::endl;
        return getRecordsFromLeafNode(node);
    }
}
