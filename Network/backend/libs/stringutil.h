#ifndef __STRING_UTIL_H__
#define __STRING_UTIL_H__

#include <common.h>

#include <string>

void splitStringToMap(const std::string& data, char sep_lev1, char sep_lev2, key_value_t& map_out);


#endif
