#include "bplusTree.h"

BPlusNode *get_root_tree(BPlusNode *cursor)
{
    if (cursor->parent_pointer == nullptr)
    {
        return cursor;
    }
    else
    {
        return get_root_tree(cursor->parent_pointer);
    }
}

void printBtree(BPlusNode *cursor, int level)
{
    if (cursor == nullptr)
    {
        return;
    }
    if (cursor->node_type != LEAF_NODE)
    {
        for (int i = 0; i < cursor->num_keys; i++)
        {
            std::cout << "level: " << level << " key: " << cursor->key_ptr_pairs[i]->second << std::endl;
            printBtree(cursor->key_ptr_pairs[i]->first, level + 1);
        }
        printBtree(cursor->right_child_ptr, level + 1);
    }
    else
    {
        for (int i = 0; i < cursor->num_records; i++)
        {
            std::cout << "------------------------------------------------" << std::endl;
            std::cout << "level: " << level << " record id: " << cursor->records[i]->first << std::endl;
            std::cout << "parent: " << (cursor->parent_pointer != nullptr ? (cursor->parent_pointer->key_ptr_pairs[0]->second) : -1) << std::endl;
        }
    }
}

void get_records_kptr(BPlusNode *cursor, std::vector<Record> &records)
{
    if (cursor == nullptr)
    {
        return;
    }

    if (cursor->node_type != LEAF_NODE)
    {
        for (int i = 0; i < cursor->num_keys; i++)
        {
            get_records_kptr(cursor->key_ptr_pairs[i]->first, records);
        }
        get_records_kptr(cursor->right_child_ptr, records);
    }
    else
    {
        for (int i = 0; i < cursor->num_records; i++)
        {
            records.push_back(deserializeRecord(cursor->records[i]->second));
        }
    }
}

BPlusNode *findLeaf(BPlusNode *cursor, int key)
{
    if (cursor->node_type == LEAF_NODE)
    {
        return cursor;
    }
    else
    {
        for (int i = 0; i < cursor->num_keys; i++)
        {
            if (key < cursor->key_ptr_pairs[i]->second)
            {
                return findLeaf(cursor->key_ptr_pairs[i]->first, key);
            }
        }
        return findLeaf(cursor->right_child_ptr, key);
    }
}

void insert_record(std::pair<int, char[VALUE_SIZE]> **records, Record record, int lenRecords)
{

    int pos = lenRecords;
    for (int i = 0; i < lenRecords; i++)
    {
        if (record.id < records[i]->first)
        {
            pos = i;
            break;
        }
    }
    for (int i = lenRecords; i > pos; i--)
    {
        records[i] = records[i - 1];
    }
    records[pos] = new std::pair<int, char[VALUE_SIZE]>;
    records[pos]->first = record.id;
    serializeRecord(record, records[pos]->second);
}

void insert_record_leaf(BPlusNode *&cursor, Record record)
{
    insert_record(cursor->records, record, cursor->num_records);
    cursor->num_records++;
}

BPlusNode *update_parent(BPlusNode *&cursor, BPlusNode *&rightNode)
{

    BPlusNode *parent = cursor->parent_pointer;
    int pos = parent->num_keys;
    for (int i = 0; i < parent->num_keys; i++)
    {
        if (cursor->records[0]->first < parent->key_ptr_pairs[i]->second)
        {
            pos = i;
            break;
        }
    }
    for (int i = parent->num_keys; i > pos; i--)
    {
        parent->key_ptr_pairs[i] = parent->key_ptr_pairs[i - 1];
    }
    parent->key_ptr_pairs[pos] = new std::pair<BPlusNode *, int>;
    parent->key_ptr_pairs[pos]->first = cursor;
    parent->key_ptr_pairs[pos]->second = rightNode->node_type == LEAF_NODE ? rightNode->records[0]->first : rightNode->key_ptr_pairs[0]->second;
    parent->num_keys++;
    parent->right_child_ptr = rightNode;
    cursor->right_child_ptr = nullptr;
    rightNode->parent_pointer = parent;

    return get_root_tree(parent);
};

BPlusNode *splitLeaf(BPlusNode *&cursor)
{
    BPlusNode *newLeaf = new BPlusNode(LEAF_NODE);

    newLeaf->right_child_ptr = nullptr;
    cursor->right_child_ptr = nullptr;
    int mid = ceil(cursor->num_records / 2);
    for (int i = mid; i < cursor->num_records; i++)
    {
        insert_record_leaf(newLeaf, deserializeRecord(cursor->records[i]->second));
    }
    cursor->num_records = mid;
    if (cursor->is_root)
    {
        BPlusNode *newRoot = new BPlusNode(INTERNAL_NODE);
        newRoot->is_root = true;
        newRoot->key_ptr_pairs[0] = new std::pair<BPlusNode *, int>;
        newRoot->key_ptr_pairs[0]->first = cursor;
        newRoot->key_ptr_pairs[0]->second = newLeaf->records[0]->first;
        newRoot->right_child_ptr = newLeaf;
        newRoot->num_keys++;
        cursor->parent_pointer = newRoot;
        newLeaf->parent_pointer = newRoot;
        cursor->is_root = false;
        newLeaf->is_root = false;
        return newRoot;
    }
    else
    {
        std::cout << "update parent" << std::endl;
        return update_parent(cursor, newLeaf);
    }
}

// debe solucionarse el problema de actualizar los parent pointers al cambiar de nodo padre
BPlusNode *splitInternal(BPlusNode *internal)
{
    BPlusNode *newInternal = new BPlusNode(INTERNAL_NODE);
    int mid = ceil(internal->num_keys / 2);
    for (int i = mid; i < internal->num_keys - 1; i++)
    {
        newInternal->key_ptr_pairs[i - mid] = internal->key_ptr_pairs[i + 1];
        newInternal->key_ptr_pairs[i - mid]->first->parent_pointer = newInternal;
        newInternal->num_keys++;
    }
    internal->num_keys = mid;
    if (internal->is_root)
    {
        newInternal->right_child_ptr = internal->right_child_ptr;
        newInternal->right_child_ptr->parent_pointer = newInternal;
        internal->right_child_ptr = internal->key_ptr_pairs[mid]->first;
        BPlusNode *newRoot = new BPlusNode(INTERNAL_NODE);
        newRoot->is_root = true;
        newRoot->key_ptr_pairs[0] = new std::pair<BPlusNode *, int>;
        newRoot->key_ptr_pairs[0]->first = internal;
        newRoot->key_ptr_pairs[0]->second = internal->key_ptr_pairs[mid]->second;
        newRoot->right_child_ptr = newInternal;
        newRoot->num_keys++;
        internal->parent_pointer = newRoot;
        newInternal->parent_pointer = newRoot;
        internal->is_root = false;
        newInternal->is_root = false;
        return newRoot;
    }
    else
    {

        return update_parent(internal, newInternal);
    }
}

void BPlusTree::Insert(Record record)
{
    BPlusNode *cursor = this->root;

    if (this->root == nullptr)
    {
        this->root = new BPlusNode(LEAF_NODE);
        this->root->is_root = true;
        insert_record_leaf(this->root, record);
        this->totalRecords++;
        this->totalNodes++;
    }
    else
    {
        cursor = findLeaf(cursor, record.id);
        insert_record_leaf(cursor, record);
        this->totalRecords++;
    }
    if (cursor != nullptr && cursor->num_records == this->leafSize)
    {
        this->root = splitLeaf(cursor);
        this->totalNodes++;
    }
    if (this->root->num_keys == this->internalSize)
    {
        this->root = splitInternal(this->root);
        this->totalNodes++;
        std::cout << "split internal" << std::endl;
    }
}

std::vector<Record> BPlusTree::GetRecords()
{
    std::vector<Record> records;
    get_records_kptr(this->root, records);
    printBtree(this->root, 0);
    return records;
}
