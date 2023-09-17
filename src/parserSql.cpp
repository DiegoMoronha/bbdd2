#include <iostream>
#include <vector>
#include <map>
#include <functional>
#include <sstream>
#include <string>
#include <algorithm>
#include <any>
#include "database.cpp"

Database database("data.db");

std::string dataTable()
{
    std::string result = "";
    result += "Paginas: " + std::to_string(database.numPages()) + "\n";
    result += "Registros: " + std::to_string(database.numRecords());

    return result;
};

std::map<std::string, std::function<std::string()>> metacommands = {
    {".table-metadata", dataTable}};

bool isMetacommand(std::string input)
{
    return input[0] == '.';
}

Record validateAndCreateRecord(std::vector<std::string> tokens)
{
    Record newRecord;
    if (tokens.size() == 4)
    {
        newRecord.id = std::stoi(tokens[1]);
        std::strcpy(newRecord.user, tokens[2].c_str());
        std::strcpy(newRecord.email, tokens[3].c_str());

        return newRecord;
    }
    throw std::invalid_argument("Missing or surplus arguments for the table columns.");
};

std::string select(std::vector<std::string> tokens)
{
    if (tokens.size() > 1)
        return "Operación inválida";

    return database.select();
};

std::string insert(std::vector<std::string> recordString)
{
    try
    {
        Record record = validateAndCreateRecord(recordString);
        database.insert(record);
        return "INSERT exitoso";
    }
    catch (const std::exception &e)
    {
        return "Operación inválida";
    }
};

std::string toLower(std::string sentence)
{
    std::transform(sentence.begin(), sentence.end(), sentence.begin(), ::tolower);
    return sentence;
};

// Concatena si el token empieza y termina con comillas
std::string concatIfQuoted(std::vector<std::string> &tokens, std::string token)
{
    if (token[0] == '"' && token[token.length() - 1] == '"')
    {
        tokens[tokens.size() - 1] += " " + token;
    }
    return token;
};

void addToken(std::vector<std::string> &tokens, std::string &token)
{
    if (!token.empty())
    {
        tokens.push_back(token);
        token.clear();
    }
}

std::vector<std::string> tokenize(const std::string &str, char delimiter)
{
    if (str == "")
        return std::vector<std::string>(0);
    std::vector<std::string> tokens;
    std::string token;
    bool insideQuotes = false;

    for (char c : toLower(str))
    {
        if (c == ' ' && !insideQuotes)
        {
            addToken(tokens, token);
        }
        else if (c == '"')
        {

            insideQuotes = !insideQuotes;
            token += c; // Agregamos la comilla al token
        }
        else
        {
            if (c == ' ')
            {
                insideQuotes = !insideQuotes;
                tokens.push_back(token);
                token.clear();
            }
            // Agregamos el carácter actual al token
            token += c;
        }
    }

    addToken(tokens, token);
    return tokens;
};

std::string executeCommands(std::vector<std::string> tokens)
{
    std::string result = "";
    if (tokens.empty())
        return result;
    if (tokens[0] == "select")
    {
        return select(tokens);
    }
    else if (tokens[0] == "insert")
    {
        return insert(tokens);
    }
    return result;
};

std::string executeMetacommand(std::string input)
{
    std::string result = "";
    try
    {
        return metacommands.at(input)();
    }
    catch (const std::exception &e)
    {
        return "Metacomando invalido";
    }
};

std::string execute(std::string input)
{
    if (isMetacommand(input))
    {
        return executeMetacommand(input);
    }
    else
    {
        std::vector<std::string> tokens;
        for (int i = 0; i < 30; i++)
        {
            tokens.push_back("insert");
            tokens.push_back(std::to_string(i));
            tokens.push_back("user");
            tokens.push_back("email");
            insert(tokens);
            tokens.clear();
        }
        return executeCommands(tokenize(input, ' '));
    }
}

std::string parse(std::string query)
{

    return execute(query);
};
