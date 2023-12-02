#include "bplusTree.h"

BPlusNode *splitInternal(BPlusNode *internal, BPlusTree *b_plus_tree);

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

void printBtree(BPlusNode *cursor, int level = 0)
{
    if (cursor == nullptr)
    {
        return;
    }
    if (cursor->node_type != LEAF_NODE)
    {
        for (int i = 0; i < cursor->num_keys; i++)
        {
            std::cout << "level: " << level << " key: " << cursor->key_ptr_pairs[i]->second << " keys: " << cursor->num_keys << std::endl;
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

// recorrer el cursor y verificar si cumple con las invariantes
BPlusNode *check_invariant(BPlusNode *cursor, BPlusTree *b_plus_tree)
{
    int limit_internal = b_plus_tree->internalSize;
    if (cursor == nullptr)
    {
        return nullptr;
    }
    if (cursor->num_keys == limit_internal)
    {
        return splitInternal(cursor, b_plus_tree);
    }
    if (cursor->parent_pointer == nullptr)
    {
        return cursor;
    }
    return check_invariant(cursor->parent_pointer, b_plus_tree);
}

BPlusNode *update_parent(BPlusNode *&cursor, BPlusNode *&rightNode, BPlusTree *b_plus_tree)
{

    BPlusNode *parent = cursor->parent_pointer;
    int pos = parent->num_keys;
    for (int i = 0; i < parent->num_keys; i++)
    {

        if (cursor->node_type == LEAF_NODE && cursor->records[0]->first < parent->key_ptr_pairs[i]->second)
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
    parent->key_ptr_pairs[pos]->second = rightNode->node_type == LEAF_NODE ? rightNode->records[0]->first : cursor->key_ptr_pairs[cursor->num_keys]->second;
    parent->num_keys++;
    parent->right_child_ptr = rightNode;
    rightNode->parent_pointer = parent;
    BPlusNode *bTree = check_invariant(parent, b_plus_tree);
    return get_root_tree(bTree);
};

BPlusNode *splitLeaf(BPlusNode *&cursor, BPlusTree *b_plus_tree)
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
        b_plus_tree->totalNodes += 2; // se suma el nodo interno mas el nuevo nodo hoja
        return newRoot;
    }
    else
    {
        return update_parent(cursor, newLeaf, b_plus_tree);
    }
}

BPlusNode *splitInternal(BPlusNode *internal, BPlusTree *b_plus_tree)
{
    BPlusNode *newInternal = new BPlusNode(INTERNAL_NODE);
    int mid = ceil(internal->num_keys / 2);
    for (int i = mid; i < internal->num_keys - 1; i++)
    {
        newInternal->key_ptr_pairs[i - mid] = internal->key_ptr_pairs[i + 1];
        newInternal->key_ptr_pairs[i - mid]->first->parent_pointer = newInternal;
        newInternal->num_keys++;
    }
    newInternal->right_child_ptr = internal->right_child_ptr;
    newInternal->right_child_ptr->parent_pointer = newInternal; // el padre de su hijo
    internal->right_child_ptr = internal->key_ptr_pairs[mid]->first;
    internal->num_keys = mid;
    b_plus_tree->totalNodes++;
    if (internal->is_root)
    {

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
        b_plus_tree->totalNodes++;
        return newRoot;
    }
    else
    {
        return update_parent(internal, newInternal, b_plus_tree);
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
        this->root = splitLeaf(cursor, this);
    }
    if (this->root->num_keys == this->internalSize)
    {
        this->root = splitInternal(this->root, this);
    }
}

std::vector<Record> BPlusTree::GetRecords()
{
    std::vector<Record> records;
    get_records_kptr(this->root, records);
    return records;
}

void BPlusTree::serializeBTree(BPlusNode *cursor, std::fstream &file, int &pos, char *buffer, int &reg)
{
    if (cursor == nullptr)
    {
        return;
    }
    if (cursor->node_type != LEAF_NODE)
    {
        for (int i = 0; i < cursor->num_keys; i++)
        {
            serializeBTree(cursor->key_ptr_pairs[i]->first, file, pos, buffer, reg);
        }
        serializeBTree(cursor->right_child_ptr, file, pos, buffer, reg);
    }
    else
    {
        for (int i = 0; i < cursor->num_records; i++)
        {
            memcpy(buffer, cursor->records[i]->second, 291);
            file.seekp(pos);
            file.write(buffer, 291);
            reg++;
            int resultado = 4096 * floor(reg / 14);
            if (resultado != 0 && ((reg % 14) == 0))
            {
                pos = resultado;
            }
            else
            {
                pos += 291;
            }
        }
    }
}

void BPlusTree::deserealizeBtree(BPlusNode *&cursor, std::fstream &file, int numRecords, int pages)
{
    char *buffer = new char[4096 * pages];
    int pos = 0;
    int originalRecords = numRecords;
    for (int i = 0; i < pages; i++)
    {
        file.seekg(pos);
        file.read(buffer, 291 * numRecords);
        if ((i >= (pages - 1) && originalRecords % 14 != 0))
        {
            numRecords = (originalRecords % 14);
        }
        else
        {
            numRecords = originalRecords % 14 == 0 ? originalRecords : 14;
        }

        for (int j = 0; j < numRecords; j++)
        {
            Record rec = deserializeRecord(&buffer[j * 291]);
            this->Insert(rec);
        }
        pos = 4096 * floor(numRecords / 14) * (i + 1);
        ;
    }
    delete buffer;
}
