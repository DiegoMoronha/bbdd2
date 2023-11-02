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
    BPlusTree node = new BPlusTreeST;
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

BPlusTree deepCopy(BPlusTree node)
{
    if (node == nullptr)
    {
        std::cout << "node is null" << std::endl;
        return nullptr;
    }

    BPlusTree copy = createNode(node->node_type);
    copy->is_root = node->is_root;
    copy->num_keys = node->num_keys;
    copy->num_records = node->num_records;
    copy->records = node->records;
    std::cout << "node type: " << (node->node_type ? "Internal" : "Leaf") << std::endl;
    if (node->node_type == INTERNAL_NODE)
    {
        std::cout << "node kptr: " << node->key_ptr_pairs.size() << std::endl;
        copy->parent_pointer = deepCopy(node->parent_pointer);
        copy->right_child_ptr = deepCopy(node->right_child_ptr);

        for (auto &keyPtrPair : node->key_ptr_pairs)
        {
            BPlusTree newChild = deepCopy(keyPtrPair.first);

            copy->key_ptr_pairs.push_back(std::make_pair(newChild, keyPtrPair.second));
        }
        return copy;
    }
    copy->parent_pointer = node->parent_pointer;
    return copy;
}

void splitInternalNode(BPlusTree &node)
{
    BPlusTree parent = deepCopy(node->parent_pointer);
    BPlusTree old_parent_right_child = parent->right_child_ptr;

    BPlusTree newInternal = createNode(INTERNAL_NODE);
    newInternal->is_root = false;
    std::vector<std::pair<BPlusTree, int>> keyPtrPairs = parent->key_ptr_pairs;
    int middle = ceil(NODE_SIZE + 1 / 2);
    for (auto it = std::next(keyPtrPairs.begin(), middle); it != keyPtrPairs.end(); it++)
    {
        parent->num_keys--;
        parent->key_ptr_pairs.erase(it);
        newInternal->num_keys++;
        newInternal->key_ptr_pairs.push_back(*it);
    }
    parent->key_ptr_pairs.erase(std::next(parent->key_ptr_pairs.begin(), middle - 1));
    parent->num_keys--;
    newInternal->right_child_ptr = old_parent_right_child;
    parent->right_child_ptr = keyPtrPairs[middle - 1].first;
    if (parent->is_root)
    {
        old_parent_right_child->is_root = false;
        parent->is_root = false;
        BPlusTree newParent = createNode(INTERNAL_NODE);
        newParent->is_root = true;
        newParent->right_child_ptr = newInternal;
        newParent->key_ptr_pairs.push_back(std::make_pair(parent, keyPtrPairs[middle - 1].second));
        newParent->num_keys++;
        parent->parent_pointer = newParent;
        newInternal->parent_pointer = newParent;
        node->parent_pointer = nullptr;
        node = newParent;
    }
    else
    {
    }
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
        std::cout << "record es mayor" << std::endl;
        parent->key_ptr_pairs.push_back(std::make_pair(parent->right_child_ptr, records[middle - 1].id));
        parent->right_child_ptr = newLeaf;
    }
    else
    {
        std::cout << "record es menor" << std::endl;
        int last_key = parent->key_ptr_pairs[parent->num_keys - 1].second;
        parent->key_ptr_pairs[parent->num_keys - 1].first = node;
        parent->key_ptr_pairs[parent->num_keys - 1].second = records[middle - 1].id;
        parent->key_ptr_pairs.push_back(std::make_pair(newLeaf, last_key));
    }
    std::sort(parent->key_ptr_pairs.begin(), parent->key_ptr_pairs.end(), [](std::pair<BPlusTree, int> a, std::pair<BPlusTree, int> b)
              { return a.second < b.second; });
    parent->num_keys++;
}
bool splitNode(BPlusTree &node, Record record)
{
    if (node->is_root)
    {
        splitLeafNodeRoot(node, record);
    }
    else
    {
        std::cout << "Splitting leaf node not root" << std::endl;
        splitLeafNode(node, record);

        if (node->parent_pointer->num_keys > NODE_SIZE)
        {
            return true;
        }
    }
    return false;
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

void splitInternalNode2(BPlusTree &node)
{
    // deepCopy(node);
    std::cout
        << "exec" << std::endl;
    BPlusTree newLeaf = createNode(LEAF_NODE);
    newLeaf->is_root = false;
    newLeaf->records.clear();
    newLeaf->num_records = 0;
    BPlusTree otherLeaf = createNode(LEAF_NODE);
    otherLeaf->is_root = false;
    otherLeaf->records.clear();
    otherLeaf->num_records = 0;
    BPlusTree newInternal = createNode(INTERNAL_NODE);
    newInternal->is_root = true;
    newInternal->key_ptr_pairs.push_back(std::make_pair(newLeaf, 9));
    newInternal->right_child_ptr = otherLeaf;
}

// Función para insertar un registro en un nodo hoja
void insertBPlus(BPlusTree &node, Record record)
{
    if (node != nullptr && node->node_type == INTERNAL_NODE)
    {
        std::cout << "internal key " << node->key_ptr_pairs[0].second << std::endl;
    }

    bool requireSplitInternal = false;
    initializeNodeIsNull(node);
    if (node->node_type == LEAF_NODE && isFullLeaf(node))
    {
        requireSplitInternal = splitNode(node, record);
    }
    else
    {
        if (node->node_type == INTERNAL_NODE)
        {
            std::cout << "Internal node number " << node->key_ptr_pairs.size() << std::endl;
            for (auto &keyPtrPair : node->key_ptr_pairs)
            {
                std::cout << "Key: " << keyPtrPair.second << std::endl;
                std::cout << "Record id: " << record.id << std::endl;
                if (record.id < keyPtrPair.second && keyPtrPair.first->node_type == LEAF_NODE)
                {
                    std::cout << "Inserting in left child" << std::endl;
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
    // printBPlusTree(node);
    if (requireSplitInternal && node->node_type == LEAF_NODE)
    {
        std::cout << "Splitting internal node" << std::endl;
        splitInternalNode(node);
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

void destroyBPlusTree(BPlusTree &node)
{
    if (node == nullptr)
    {
        return;
    }

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
void checkRightChild(BPlusTree &parent, int key_to_insert, BPlusTree &new_node)
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
void splitInternalNodeV2(BPlusTree &node)
{
    BPlusTree new_node = createNode(INTERNAL_NODE);
    int mid_index = node->num_keys / 2;
    int mid_key = node->key_ptr_pairs[mid_index].second;

    new_node->key_ptr_pairs.insert(new_node->key_ptr_pairs.begin(), node->key_ptr_pairs.begin() + mid_index + 1, node->key_ptr_pairs.end());
    node->key_ptr_pairs.erase(node->key_ptr_pairs.begin() + mid_index, node->key_ptr_pairs.end());
    node->num_keys = node->key_ptr_pairs.size();
    new_node->num_keys = new_node->key_ptr_pairs.size();

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
        printBPlusTree(new_root);
        node = new_root; // Actualiza el puntero del nodo a la nueva raíz
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
            splitInternalNodeV2(parent);
        }
    }
}

void splitLeafNodeV2(BPlusTree &node)
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

        node = new_root; // Actualiza el puntero del nodo a la nueva raíz
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
            splitInternalNodeV2(parent);
        }
    }
}

void insertInLeaf(BPlusTree &root, Record record)
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
            splitLeafNodeV2(root);
            return;
        }
    }
    else
    {
        for (auto &keyPtrPair : root->key_ptr_pairs)
        {
            if (record.id < keyPtrPair.second)
            {
                insertB(keyPtrPair.first, record);
                return;
            }
        }
        insertB(root->right_child_ptr, record);
    }
}
