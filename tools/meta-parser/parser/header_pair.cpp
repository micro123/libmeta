#include "header_pair.hpp"
#include <cstdio>
#include <cstring>

static char* TrimString(char *input)
{
    auto len = strlen(input);
    while(*input && isspace(*input))
    {
        ++input;
        --len;
    }
    char *end = input + len - 1;
    while(*end && isspace(*end))
    {
        *end = '\0';
        --len;
        --end;
    }
    return input;
}

std::list<std::pair<std::string, std::string>> ParseHeaderPairFile (const char *path)
{
    std::list<std::pair<std::string, std::string>> result;
    FILE *f = fopen(path, "r");
    if (f)
    {
        char line_buffer[1024] {};
        while(fgets(line_buffer, sizeof(line_buffer), f))
        {
            char *line = TrimString(line_buffer);
            char *split = strstr(line_buffer, "|>");
            if (!split)
                continue;
            
            *split = 0;
            split += 2;
            
            result.emplace_back(line_buffer, split);
        }
        fclose(f);
    }
    return result;
}