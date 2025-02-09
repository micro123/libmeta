#ifndef STR_UTILS_HPP
#define STR_UTILS_HPP

#include <string>
#include <vector>

std::vector<std::string> split(std::string input, const std::string& pat);

std::string replace(std::string& source_string, const std::string& sub_string, const std::string& new_string);

std::string replace(std::string& source_string, char target_char, char new_char);

std::string toUpper(std::string& source_string);

std::string join (const std::vector<std::string> &context_list, const std::string& separator);

std::string trim(std::string& source_string, const std::string& trim_chars);


#endif  // STR_UTILS_HPP
