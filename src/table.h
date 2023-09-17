#include <vector>
#include <string>
#include <cstring>
#include "record.h"
#include "pager.cpp"

struct Table
{
    int totalRecords;
    Pager *pager;
};

Record getRecord(int pageIndex, int recordIndex, Table table);
std::vector<Record> getRecords(int pageIndex, Table table);
void addRecordToTable(Table &table, Record record);
