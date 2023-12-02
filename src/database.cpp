#include <string>
#include <iostream>

#include <vector>
#include "bplusTree.h"
#include "pager.cpp"
class Database
{
private:
    // Table table;
    BPlusTree bPlusTree = BPlusTree();
    Pager pager;

public:
    Database(const std::string &filename) : pager(Pager(filename)) {}

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
        bool open = pager.openFile();
        int numpages;
        int numrecords;
        pager.calculateMetadata(numpages, numrecords);
        std::cout << "num pages" << numpages << std::endl;
        std::cout << "num records" << numrecords << std::endl;
        bPlusTree.deserealizeBtree(bPlusTree.root, pager.getFile(), numrecords, numpages);
        return open;
    }

    void closeDatabase()
    {

        auto &file = pager.getFile();
        file.close();
        pager.createOrWriteFile();
        const int size = bPlusTree.totalRecords * 291;
        char *buffer = new char[size];
        int pos = 0;
        int reg = 0;
        bPlusTree.serializeBTree(bPlusTree.root, file, pos, buffer, reg);
        delete buffer;
        file.close();
    }

    int numRecords()
    {
        return bPlusTree.totalRecords;
    }
};
