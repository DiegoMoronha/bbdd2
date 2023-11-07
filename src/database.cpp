#include <string>
#include <iostream>

#include <vector>
#include "bplusTree.h"
// #include "table.h"
class Database
{
private:
    // Table table;
    BPlusTree bPlusTree = BPlusTree();

public:
    Database(const std::string &filename) {} /* table({0, new Pager(filename)})*/

    std::string select()
    {
        std::string result = "";
        std::vector<Record> pageRecords = bPlusTree.GetRecords();
        for (int j = 0; j < bPlusTree.totalRecords; j++)
        {

            if (j == bPlusTree.totalRecords - 1)
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
        bPlusTree.Insert(record);
    }

    void deletePager(){
        //  delete table.pager;
    };

    int pages()
    {
        return bPlusTree.totalNodes;
    }

    bool openDatabase()
    {
        return true;
    }

    void closeDatabase()
    {
    }

    int numRecords()
    {
        return bPlusTree.totalRecords;
    }
};
