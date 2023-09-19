#include <iostream>
#include <algorithm>
#include <string>
#include "parserSql.h"

void printResult(std::string result)
{
    if (result != "")
        std::cout << result << std::endl;
};

int replSql(char *argv[])
{
    Database database(argv[1]);
    database.openDatabase();
    std::string input;

    while (true)
    {
        std::cout << "sql>";
        std::getline(std::cin, input);
        if (input == ".exit")
        {
            database.closeDatabase();
            std::cout << "Terminado" << std::endl;
            break;
        }
        printResult(parse(input, database));
    }

    return 0;
}
