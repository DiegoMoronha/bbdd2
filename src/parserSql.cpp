#include <iostream>
#include <vector>
#include <sstream>
#include <string>
#include <algorithm> // Para std::transform

std::string toLower(std::string sentence)
{
    std::transform(sentence.begin(), sentence.end(), sentence.begin(), ::tolower);
    return sentence;
};

// Función para dividir una cadena en tokens
std::vector<std::string> tokenize(const std::string &str, char delimiter)
{
    std::vector<std::string> tokens;
    std::istringstream tokenStream(toLower(str));
    std::string token;
    while (std::getline(tokenStream, token, delimiter))
    {
        tokens.push_back(token);
    }
    return tokens;
};

std::string parse(std::string sql)
{
    std::string result = "";
    std::vector<std::string> tokens = tokenize(sql, ' ');
    if (tokens[0] == "select")
    {
        result = "SELECT no implementado";
    }
    else if (tokens[0] == "insert")
    {
        result = "INSERT no implementado";
    }
    else
    {
        result = "";
    }
    return result;
};
