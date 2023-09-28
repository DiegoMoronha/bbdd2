#include <string>
#include <iostream>

#include <vector>
#include "bplusTree.h"
// #include "table.h"
class Database
{
private:
    // Table table;
    BPlusTree *bPlusTree = new BPlusTree;

public:
    Database(const std::string &filename) {} /* table({0, new Pager(filename)})*/
    int numPages;

    std::string select()
    {
        std::string result = "";
        for (int i = 0; i < pages(); i++)
        {
            std::vector<Record> pageRecords = getRecordsFromLeafNode(bPlusTree);
            for (int j = 0; j < bPlusTree->num_records; j++)
            {

                if (j == bPlusTree->num_records - 1)
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
        insertIntoLeafNode(bPlusTree, record);
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

        initializeLeafNode(bPlusTree);
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
