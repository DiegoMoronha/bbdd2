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

bool isFullLeaf(BPlusTree &node)
{
    return node->num_records >= MAX_RECORDS;
}

BPlusTree initializeNodeIsNull(BPlusTree &node)
{
    if (node == nullptr)
    {
        node = createNode(LEAF_NODE);
        node->is_root = true;
    }
    return node;
}

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
            if (keyPtrPair.first->node_type == LEAF_NODE)
            {
                std::vector<Record> recordsFromChild = getRecordsFromLeafNode(keyPtrPair.first);
                records.insert(records.end(), recordsFromChild.begin(), recordsFromChild.end());
            }
            else
            {
                std::vector<Record> recordsFromChild = getRecordsFromLeavesNode(keyPtrPair.first);
                records.insert(records.end(), recordsFromChild.begin(), recordsFromChild.end());
            }
        }

        if (node->right_child_ptr->node_type == LEAF_NODE)
        {
            std::vector<Record> recordsFromRightChild = getRecordsFromLeafNode(node->right_child_ptr);
            records.insert(records.end(), recordsFromRightChild.begin(), recordsFromRightChild.end());
        }
        else
        {
            std::vector<Record> recordsFromRightChild = getRecordsFromLeavesNode(node->right_child_ptr);
            records.insert(records.end(), recordsFromRightChild.begin(), recordsFromRightChild.end());
        }
    }
    else
    {
        return getRecordsFromLeafNode(node);
    }
    return records;
}

std::vector<Record> getRecords(BPlusTree node)
{
    return getRecordsFromLeavesNode(node);
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

void printBPlusTree(BPlusTree node)
{
    if (node == nullptr)
    {
        std::cout << "Node is null" << std::endl;
        return;
    }

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
        std::cout << "Internal node: " << node->key_ptr_pairs[0].second << std::endl;
        std::cout << "Num keys: " << node->num_keys << std::endl;
        for (auto &keyPtrPair : node->key_ptr_pairs)
        {
            std::cout << "Left child of: " << keyPtrPair.second << std::endl;
            printBPlusTree(keyPtrPair.first);
        }
        if (node->right_child_ptr != nullptr)
        {
            std::cout << "Right child of: " << node->key_ptr_pairs[node->num_keys - 1].second << std::endl;

            printBPlusTree(node->right_child_ptr);
        }
    }
}

//
//
//
//
//
//
//
//
//
//
void checkRightChild(BPlusTree parent, int key_to_insert, BPlusTree new_node)
{
    if (parent->key_ptr_pairs[parent->num_keys - 1].second < key_to_insert)
    {
        parent->key_ptr_pairs.push_back({parent->right_child_ptr, key_to_insert});
        parent->right_child_ptr = new_node;
    }
    else
    {
        parent->key_ptr_pairs.push_back({new_node, key_to_insert});
    }
    new_node->parent_pointer = parent;
}

BPlusTree splitInternalNodeV2(BPlusTree node)
{
    BPlusTree new_node = createNode(INTERNAL_NODE);
    int mid_index = node->num_keys / 2;
    int mid_key = node->key_ptr_pairs[mid_index].second;
    BPlusTree left_child = node->key_ptr_pairs[mid_index].first;
    BPlusTree right_child = node->right_child_ptr;
    node->right_child_ptr = left_child;
    new_node->key_ptr_pairs.insert(new_node->key_ptr_pairs.begin(), node->key_ptr_pairs.begin() + mid_index + 1, node->key_ptr_pairs.end());
    node->key_ptr_pairs.erase(node->key_ptr_pairs.begin() + mid_index, node->key_ptr_pairs.end());
    node->num_keys = node->key_ptr_pairs.size();
    new_node->num_keys = new_node->key_ptr_pairs.size();
    new_node->right_child_ptr = right_child;
    if (node->is_root)
    {
        BPlusTree new_root = createNode(INTERNAL_NODE);
        new_root->is_root = true;
        new_root->num_keys++;
        new_root->key_ptr_pairs.push_back({node, mid_key});
        new_root->right_child_ptr = new_node;
        node->is_root = false;
        node->parent_pointer = new_root;
        new_node->parent_pointer = new_root;
        std::cout << "es root split internal" << std::endl;
        return new_root; // Actualiza el puntero del nodo a la nueva raíz
    }
    else
    {
        BPlusTree parent = node->parent_pointer;
        checkRightChild(parent, mid_key, new_node);
        parent->num_keys++;
        std::sort(parent->key_ptr_pairs.begin(), parent->key_ptr_pairs.end(), [](std::pair<BPlusTree, int> a, std::pair<BPlusTree, int> b)
                  { return a.second < b.second; });

        if (parent->num_keys > NODE_SIZE)
        {
            return splitInternalNodeV2(parent);
        }
    }
}

BPlusTree splitLeafNodeV2(BPlusTree node)
{
    BPlusTree new_node = createNode(LEAF_NODE);

    int mid_index = node->num_records / 2;
    auto it = node->records.begin();
    std::advance(it, mid_index);

    new_node->records.insert(it, node->records.end());
    node->records.erase(it, node->records.end());
    node->num_records = node->records.size();
    new_node->num_records = new_node->records.size();
    if (node->is_root)
    {

        BPlusTree new_root = createNode(INTERNAL_NODE);
        new_root->is_root = true;
        new_root->num_keys++;
        new_root->key_ptr_pairs.push_back({node, it->first});
        new_root->right_child_ptr = new_node;
        node->is_root = false;
        node->parent_pointer = new_root;
        new_node->parent_pointer = new_root;
        return new_root; // Actualiza el puntero del nodo a la nueva raíz
    }
    else
    {
        int key_to_insert = it->first;

        BPlusTree parent = node->parent_pointer;
        checkRightChild(parent, key_to_insert, new_node);
        parent->num_keys++;
        std::sort(parent->key_ptr_pairs.begin(), parent->key_ptr_pairs.end(), [](std::pair<BPlusTree, int> a, std::pair<BPlusTree, int> b)
                  { return a.second < b.second; });

        if (parent->num_keys > NODE_SIZE)
        {

            return splitInternalNodeV2(parent);
        }

        return new_node;
    }
}

void insertInLeaf(BPlusTree root, Record record)
{
    serializeRecord(record, root->records[record.id]);
    root->num_records++;
}

void insertB(BPlusTree &root, Record record)
{
    initializeNodeIsNull(root);
    if (root->node_type == LEAF_NODE)
    {
        insertInLeaf(root, record);
        if (isFullLeaf(root))
        {

            root = splitLeafNodeV2(root);
            return;
            ;
        }
    }
    else
    {
        for (auto &keyPtrPair : root->key_ptr_pairs)
        {
            if (record.id < keyPtrPair.second)
            {

                insertB(keyPtrPair.first, record);
            }
        }
        insertB(root->right_child_ptr, record);
    }
}
