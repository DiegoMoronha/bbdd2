#include <iostream>
#include <algorithm> // Para std::transform
#include <string>
#include "parserSql.h"

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
        if (parse(input) != "")
        {
            std::cout << parse(input) << std::endl;
        }
    }

    return 0;
}