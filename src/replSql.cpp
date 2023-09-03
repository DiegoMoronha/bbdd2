#include <iostream>
#include <algorithm> // Para std::transform
#include <string>
#include "parserSql.h"

void printResult(std::string result)
{
    if (result != "")
        std::cout << result << std::endl;
};

/*
void printArray(std::vector<std::string> array)
{
    std::cout << "Mi vector: [";
    bool primero = true;
    for (const auto &elemento : array)
    {
        if (!primero)
        {
            std::cout << ", ";
        }
        std::cout << elemento;
        primero = false;
    }
    std::cout << "]" << std::endl;
};
*/
int replSql()
{
    std::string input;

    while (true)
    {
        std::cout << "sql>";
        std::getline(std::cin, input);
        if (input == ".exit")
        {
            std::cout << "Terminado" << std::endl;
            break;
        }
        printResult(parse(input));
    }

    return 0;
}
