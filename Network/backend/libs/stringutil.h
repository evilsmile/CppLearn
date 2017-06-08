#ifndef __STRING_UTIL_H__
#define __STRING_UTIL_H__

#include <common.h>

#include <string>
#include <vector>

void splitString(const std::string& data, char sep, std::vector<std::string>& out);

void splitStringToMap(const std::string& data, char sep_lev1, char sep_lev2, key_value_t& map_out);


#endif
