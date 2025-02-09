#include "file_utils.hpp"

#include <cstdio>

void EmptyFile (const std::string &filePath)
{
    FILE *file = fopen (filePath.c_str (), "w");
    fclose (file);
}
