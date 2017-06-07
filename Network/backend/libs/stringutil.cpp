#include <log.h>

#include "stringutil.h"

void splitStringToMap(const std::string& data, char sep_lev1, char sep_lev2, key_value_t& map_out)
{
		std::string key, value;

		std::size_t key_start = 0, key_end, value_start, value_end;
		bool the_end = false;
		while (!the_end) 
		{
			value_end = data.find(sep_lev1, key_start);
			if (value_end == std::string::npos) {
					the_end = true;
			}
			value_end -= 1;
			key_end = data.find(sep_lev2, key_start);
			if (key_end == std::string::npos) {
					continue;
			}
			value_start = key_end + 1;
			key_end -= 1;

			key = data.substr(key_start, key_end - key_start + 1);
			value = data.substr(value_start, value_end - value_start + 1);

			key_start = value_end + 2;
			map_out[key] = value;
		}	

		for (key_value_t::iterator iter = map_out.begin();
						iter != map_out.end();
						++iter) 
		{
			log_trace("key: [%s] value: [%s]", iter->first.c_str(), iter->second.c_str());
		}

}


