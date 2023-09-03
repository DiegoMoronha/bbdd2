#include "table.h"

Record getRecord(int pageIndex, int recordIndex, Table table)
{
    // Obtener el bloque de bytes que contiene el record
    const char *recordBuffer = &table.pages[pageIndex].records[recordIndex * BLOCK_SIZE];

    // Deserializar el record
    return deserializeRecord(recordBuffer);
}

std::vector<Record> getRecords(int pageIndex, Table table)
{
    std::vector<Record> records;
    const char *recordBuffer = table.pages[pageIndex].records;
    for (int i = 0; i < table.pages[pageIndex].numRecords; i++)
    {
        records.push_back(deserializeRecord(&recordBuffer[i * BLOCK_SIZE]));
    }

    return records;
}

// add record to table params table and record
void addRecordToTable(Table &table, Record record)
{
    // Serializar el record
    char registroBuffer[BLOCK_SIZE];
    serializeRecord(record, registroBuffer);
    // Encontrar o crear una nueva página
    if (table.pages.empty() || table.pages.back().numRecords >= PAGE_SIZE / BLOCK_SIZE)
    {
        Page newPage;
        newPage.numRecords = 0;
        table.pages.push_back(newPage); // Nueva página con 0 registros inicialmente
    }

    // Agregar el record a la página actual
    Page &currentPage = table.pages.back();
    std::memcpy(&currentPage.records[currentPage.numRecords * BLOCK_SIZE],
                registroBuffer, BLOCK_SIZE);
    table.totalRecords++;
    currentPage.numRecords++;
}