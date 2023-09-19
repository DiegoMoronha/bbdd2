#include <string>
#include <vector>
#include "database.cpp"

std::vector<std::string> tokenize(const std::string &str, char delimiter);
std::string parse(std::string query, Database &database);
