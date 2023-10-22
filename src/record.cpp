#include <cstring>
#include "record.h"

void copyToBuffer(char *buffer, const char *string, size_t stringSize, int &offset)
{
    strncpy(&buffer[offset], string, stringSize);
    offset += stringSize - 1;
    buffer[offset++] = '\0';
}

void readFromBuffer(const char *buffer, char *string, size_t stringSize, int &offset)
{
    strncpy(string, &buffer[offset], stringSize);
    offset += stringSize;
}

void intToBigEndianBytes(int number, char *buffer, int &offset)
{
    buffer[offset++] = (number >> 24) & 0xFF;
    buffer[offset++] = (number >> 16) & 0xFF;
    buffer[offset++] = (number >> 8) & 0xFF;
    buffer[offset++] = number & 0xFF;
}

void bigEndianBytesToInt(int &number, const char *buffer, int &offset)
{
    number = (static_cast<unsigned char>(buffer[offset]) << 24) |
             (static_cast<unsigned char>(buffer[offset + 1]) << 16) |
             (static_cast<unsigned char>(buffer[offset + 2]) << 8) |
             static_cast<unsigned char>(buffer[offset + 3]);
    offset += 4;
}

void serializeRecord(const Record &record, char *buffer)
{
    int offset = 0;
    intToBigEndianBytes(record.id, buffer, offset);
    copyToBuffer(buffer, record.user, sizeof(record.user), offset);
    copyToBuffer(buffer, record.email, sizeof(record.email), offset);
}

Record deserializeRecord(const char *buffer)
{
    Record record;
    int offset = 0;
    bigEndianBytesToInt(record.id, buffer, offset);
    readFromBuffer(buffer, record.user, sizeof(record.user), offset);
    readFromBuffer(buffer, record.email, sizeof(record.email), offset);

    return record;
}

Record createRecord(int id, const char *user, const char *email)
{
    Record record;
    record.id = id;
    std::strcpy(record.user, user);
    std::strcpy(record.email, email);
    return record;
}