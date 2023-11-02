#include <string>
#include <iostream>

#include <vector>
#include "bplusTree.h"
// #include "table.h"
class Database
{
private:
    // Table table;
    BPlusTree bPlusTree = nullptr;

public:
    Database(const std::string &filename) {} /* table({0, new Pager(filename)})*/

    std::string select()
    {
        std::string result = "";
        std::vector<Record> pageRecords = getRecords(bPlusTree);
        for (size_t j = 0; j < pageRecords.size(); j++)
        {

            if (j == pageRecords.size() - 1)
            {
                result += std::to_string(pageRecords[j].id) + " " + pageRecords[j].user + " " + pageRecords[j].email;
                break;
            }
            result += std::to_string(pageRecords[j].id) + " " + pageRecords[j].user + " " + pageRecords[j].email + "\n";
        }
        return result;
    }

    void insert(Record record)
    {
        insertB(bPlusTree, record);
    }

    void deletePager(){
        //  delete table.pager;
    };

    int pages()
    {
        return bPlusTree->num_keys + 1;
    }

    bool openDatabase()
    {
        for (int i = 3; i < 7; i++)
        {
            auto record = createRecord(i, "user", "email");
            insert(record);
        }
        return true;
    }

    void closeDatabase()
    {
        destroyBPlusTree(bPlusTree);
    }

    int numRecords()
    {
        printBPlusTree(bPlusTree);
        return countNumRecords(bPlusTree);
    }
};
