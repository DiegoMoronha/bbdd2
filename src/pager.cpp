#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include "page.h"

class Pager
{
private:
    std::map<int, Page *> cache;
    std::string filename;
    std::fstream file;

public:
    Pager(const std::string &filename) : filename(filename) {}

    Page *createPage()
    {
        Page *page = new Page;
        page->numRecords = 0;
        page->records = new char[PAGE_SIZE];
        return page;
    }

    bool openFile()
    {
        file.open(filename, std::ios::in | std::ios::binary);
        return file.is_open();
    }

    bool createOrWriteFile()
    {
        file.open(filename, std::ios::out | std::ios::binary);
        return file.is_open();
    }

    void closeFile()
    {
        file.close();
        if (cache.size() > 0)
        {
            createOrWriteFile();
        }
        writeAllPagesToFile();
        file.close();
    }

    Page *getPage(int i)
    {
        auto it = cache.find(i);
        if (it != cache.end())
        {

            return it->second;
        }

        if (existPage(i))
        {
            Page *page = createPage();
            readPageFromFile(i, *page);
            cache[i] = page;
            return cache[i];
        }

        cache[i] = createPage();

        return cache[i];
    }

    int numPages() const
    {
        return cache.size();
    }

    void addPage(Page *&page)
    {
        int pageNumber = cache.size();
        cache[pageNumber] = page;
    }

    void writePageToFile(int pageNum, const Page &page)
    {
        int pos = pageNum * PAGE_SIZE;
        file.seekp(pos);

        file.write(page.records, page.numRecords * 291);
    }

    std::fstream &getFile()
    {
        return file;
    }

    void readPageFromFile(int pageNum, Page &page)
    {
        setPageRecords(page, pageNum);
        int pos = pageNum * PAGE_SIZE;
        file.seekg(pos);
        file.read(page.records, page.numRecords * 291);
    }

    void readFile(char *buffer, int size)
    {
        file.open(filename, std::ios::in | std::ios::binary);

        file.read(buffer, size);
    }

    void setPageRecords(Page &page, int pageNum)
    {
        int totalPages;
        int totalRecords;
        calculateMetadata(totalPages, totalRecords);
        if (totalPages > pageNum + 1)
        {
            page.numRecords = 14;
        }
        else
        {
            page.numRecords = (totalRecords % 14);
        }
    }

    bool existPage(int pageNum)
    {
        int totalPages;
        int numrecords;
        calculateMetadata(totalPages, numrecords);
        return pageNum <= totalPages;
    }

    void writeAllPagesToFile()
    {
        for (auto &entry : cache)
        {
            int pageNumber = entry.first;
            Page *page = entry.second;
            writePageToFile(pageNumber, *page);
            delete entry.second;
        }
    }

    int calculateMetadata(int &numPages, int &numRecords)
    {
        file.seekg(0, std::ios::end);
        int fileSize = file.tellg();
        numPages = (fileSize % PAGE_SIZE == 0) ? (fileSize / PAGE_SIZE) : (fileSize / PAGE_SIZE + 1);
        numRecords = (fileSize % PAGE_SIZE == 0) ? (numPages * 14) : ((fileSize / PAGE_SIZE) * 14 + (fileSize % PAGE_SIZE) / 291);
        return fileSize;
    }
};