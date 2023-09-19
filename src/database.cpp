#include <string>
#include <vector>
#include "table.h"

class Database
{
private:
    Table table;

public:
    Database(const std::string &filename) : table({0, new Pager(filename)}) {}
    int numPages;

    std::string select()
    {
        std::string result = "";
        std::vector<std::string> records;
        for (int i = 0; i < pages(); i++)
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

    int pages()
    {
        return numPages == 0 ? table.pager->numPages() : numPages;
    }

    bool openDatabase()
    {
        bool open = table.pager->openFile();
        if (!open)
        {
            return table.pager->createOrWriteFile();
        }
        table.pager->calculateMetadata(numPages, table.totalRecords);
        return open;
    }

    void closeDatabase()
    {
        table.pager->closeFile();
    }

    int numRecords()
    {
        return table.totalRecords;
    }
};
