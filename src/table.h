#include <vector>
#include <string>
#include <cstring>
#include "record.h"
#include "page.h"

struct Table
{
    int totalRecords;
    std::vector<Page> pages;
};

Record getRecord(int pageIndex, int recordIndex, Table table);
std::vector<Record> getRecords(int pageIndex, Table table);
void addRecordToTable(Table &table, Record record);
