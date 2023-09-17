#include <string>
#include <vector>
#include "table.h"

class Database
{
private:
    Table table;

public:
    Database(const std::string &filename) : table({0, new Pager(filename)}) {}

    std::string select()
    {
        std::string result = "";
        std::vector<std::string> records;
        for (int i = 0; i < table.pager->numPages(); i++)
        {
            std::vector<Record> pageRecords = getRecords(i, table);
            for (int j = 0; j < table.pager->getPage(i)->numRecords; j++)
            {

                if (j == table.pager->getPage(i)->numRecords - 1)
                {
                    result += std::to_string(pageRecords[j].id) + " " + pageRecords[j].user + " " + pageRecords[j].email;
                    break;
                }
                result += std::to_string(pageRecords[j].id) + " " + pageRecords[j].user + " " + pageRecords[j].email + "\n";
            }
            if (i != table.pager->numPages() - 1)
            {
                result += "\n";
            }
        }
        return result;
    }

    void insert(Record record)
    {
        addRecordToTable(table, record);
    }

    void deletePager()
    {
        delete table.pager;
    };

    void openDatabase()
    {
        table.pager->openFile();
    }

    void closeDatabase()
    {
        table.pager->closeFile();
    }

    int numPages()
    {
        return table.pager->numPages();
    }

    int numRecords()
    {
        return table.totalRecords;
    }
};
