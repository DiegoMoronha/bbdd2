#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include "page.h"

class Pager
{
private:
    std::map<int, Page *> cache; // Mapa para almacenar páginas en caché
    std::string filename;        // Nombre del archivo de la base de datos
    std::fstream file;           // Stream para manejar el archivo

public:
    Pager(const std::string &filename) : filename(filename) {}

    bool openFile()
    {
        file.open(filename, std::ios::in | std::ios::out | std::ios::binary);
        return file.is_open();
    }

    void closeFile()
    {
        file.close();
    }

    Page *getPage(int i)
    {
        auto it = cache.find(i);
        if (it != cache.end())
        {

            return it->second; // Devolver puntero a la página en caché
        }
        // else
        // {
        //   Page page;
        //   readPageFromFile(file, i, page);
        //   if (page.numRecords == 0)
        //    {
        //       page.numRecords = 0;
        //    }
        std::cout << "New page in get Page" << std::endl;
        Page *page = new Page;
        page->numRecords = 0;
        page->records = new char[PAGE_SIZE];
        cache[i] = page; // Guardar página en caché

        return cache[i];
        // }
    }

    int numPages() const
    {
        return cache.size(); // Retorna la cantidad de páginas en la caché
    }

    void addPage(Page *&page)
    {
        int pageNumber = cache.size(); // Página siguiente en la secuencia
        cache[pageNumber] = page;
    }

    void writePageToFile(std::fstream &file, int pageNum, const Page &page)
    {
        // Calcular posición de la página en el archivo
        int pos = pageNum * sizeof(Page);

        // Ir a la posición correspondiente en el archivo
        file.seekp(pos);

        // Escribir la página en el archivo
        file.write(reinterpret_cast<const char *>(&page), sizeof(Page));
    }

    void readPageFromFile(std::fstream &file, int pageNum, Page &page)
    {
        // Calcular posición de la página en el archivo
        int pos = pageNum * sizeof(Page);

        // Ir a la posición correspondiente en el archivo
        file.seekg(pos);

        // Leer la página desde el archivo
        file.read(reinterpret_cast<char *>(&page), sizeof(Page));
    }

    void calculateMetadata(std::fstream &file, int numPages, int numRecords)
    {
        file.seekg(0, std::ios::end); // Mover el puntero al final del archivo
        int fileSize = file.tellg();  // Obtener el tamaño actual del archivo

        numPages = (fileSize % PAGE_SIZE == 0) ? (fileSize / PAGE_SIZE) : (fileSize / PAGE_SIZE + 1);
        numRecords = (fileSize % PAGE_SIZE == 0) ? (numPages * 14) : (numPages * 14 + (fileSize % PAGE_SIZE) / 291);

        // Guardar la metadata en el archivo (si es necesario)
        // En este caso, no guardaremos la metadata, pero puedes hacerlo si lo necesitas
    }
};