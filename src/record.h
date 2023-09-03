#include <vector>
#include <string>
#define BLOCK_SIZE 291

struct Record
{
    int id;
    char user[32];
    char email[255];
};

void serializeRecord(const Record &record, char *buffer);
Record deserializeRecord(const char *buffer);
Record createRecord(int id, const char *user, const char *email);
