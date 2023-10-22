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
    int numPages;

    std::string select()
    {
        std::string result = "";
        for (int i = 0; i < 1; i++)
        {
            std::vector<Record> pageRecords = getRecordsFromLeavesNode(bPlusTree);
            for (size_t j = 0; j < pageRecords.size(); j++)
            {

                if (j == pageRecords.size() - 1)
                {
                    result += std::to_string(pageRecords[j].id) + " " + pageRecords[j].user + " " + pageRecords[j].email;
                    break;
                }
                result += std::to_string(pageRecords[j].id) + " " + pageRecords[j].user + " " + pageRecords[j].email + "\n";
            }
            /*if (i != table.pager->numPages() - 1)
            {
                result += "\n";
            }*/
        }
        return result;
    }

    void insert(Record record)
    {
        insertBPlus(bPlusTree, record);
    }

    void deletePager(){
        //  delete table.pager;
    };

    int pages()
    {
        // return numPages == 0 ? table.pager->numPages() : numPages;
        return bPlusTree->num_records == 0 ? 0 : 1;
    }

    bool openDatabase()
    {
        for (int i = 0; i < 25; i++)
        {
            Record record = createRecord(i, "user", "email");
            insert(record);
        }

        return true;
    }

    void closeDatabase()
    {
        delete bPlusTree;
    }

    int numRecords()
    {
        return bPlusTree->num_records;
    }
};
