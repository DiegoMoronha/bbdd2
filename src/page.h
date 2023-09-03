#define PAGE_SIZE 4096

struct Page
{
    int numRecords;
    char records[PAGE_SIZE];
};