#include "str_utils.hpp"
#include <algorithm>

std::vector<std::string> split (std::string input, const std::string &pat)
{
    std::vector<std::string> ret_list;
    while (true)
    {
        size_t      index    = input.find (pat);
        std::string sub_list = input.substr (0, index);
        if (!sub_list.empty ())
        {

            ret_list.push_back (sub_list);
        }
        input.erase (0, index + pat.size ());
        if (index == -1)
        {
            break;
        }
    }
    return ret_list;
}

std::string replace (std::string &source_string, const std::string &sub_string, const std::string &new_string)
{
    std::string::size_type pos = 0;
    while ((pos = source_string.find (sub_string)) != std::string::npos)
    {
        source_string.replace (pos, sub_string.length (), new_string);
    }
    return source_string;
}

std::string replace (std::string &source_string, char target_char, char new_char)
{
    std::ranges::replace (source_string, target_char, new_char);
    return source_string;
}

std::string toUpper (std::string &source_string)
{
    std::ranges::transform (source_string, source_string.begin (), ::toupper);
    return source_string;
}

std::string join (const std::vector<std::string> &context_list, const std::string &separator)
{
    std::string ret_string;
    if (context_list.empty())
    {
        return ret_string;
    }
    ret_string = context_list[0];
    for (int index = 1; index < context_list.size (); ++index)
    {
        ret_string += separator + context_list[index];
    }

    return ret_string;
}

std::string trim (std::string source_string, const std::string &trim_chars)
{
    size_t left_pos = source_string.find_first_not_of (trim_chars);
    if (left_pos == std::string::npos)
    {
        source_string = std::string ();
    }
    else
    {
        size_t right_pos = source_string.find_last_not_of (trim_chars);
        source_string    = source_string.substr (left_pos, right_pos - left_pos + 1);
    }
    return source_string;
}

std::string toCamelCase (const std::string &input)
{
    auto parts = split(input, "_");
    std::string result;
    bool use_upper_case = parts.size() > 1;
    for (auto &x: parts)
    {
        if (x.empty())
            continue;
        if (use_upper_case)
            x[0] = std::toupper(x[0]);
        result.append(x);
    }
    return result;
}
