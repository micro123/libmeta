#ifndef HEADER_PAIR_HPP
#define HEADER_PAIR_HPP

#include <string>
#include <list>
#include <utility>

std::list<std::pair<std::string, std::string>> ParseHeaderPairFile(const char *path);

#endif /* HEADER_PAIR_HPP */
