#include "table.h"

Record getRecord(int pageIndex, int recordIndex, Table table)
{
    Page *currentPage = table.pager->getPage(pageIndex);
    const char *recordBuffer = &currentPage->records[recordIndex * BLOCK_SIZE];
    return deserializeRecord(recordBuffer);
}

std::vector<Record> getRecords(int pageIndex, Table table)
{
    std::vector<Record> records;
    Page *currentPage = table.pager->getPage(pageIndex);
    const char *recordBuffer = currentPage->records;
    for (int i = 0; i < currentPage->numRecords; i++)
    {
        records.push_back(deserializeRecord(&recordBuffer[i * BLOCK_SIZE]));
    }
    return records;
}
void addRecordToTable(Table &table, Record record)
{
    Page *currentPage;
    char registroBuffer[BLOCK_SIZE];
    serializeRecord(record, registroBuffer);
    if (table.pager->numPages() > 0)
    {
        currentPage = table.pager->getPage(table.pager->numPages() - 1);
    }
    else
    {
        currentPage = table.pager->getPage(table.pager->numPages());
    }

    if (currentPage->numRecords >= PAGE_SIZE / BLOCK_SIZE)
    {
        Page *newPage = new Page;
        newPage->numRecords = 0;
        newPage->records = new char[PAGE_SIZE];
        table.pager->addPage(newPage);
        currentPage = table.pager->getPage(table.pager->numPages() - 1);
    }

    std::memcpy(&currentPage->records[currentPage->numRecords * BLOCK_SIZE],
                registroBuffer, BLOCK_SIZE);
    currentPage->numRecords = currentPage->numRecords + 1;
    table.totalRecords++;
}
