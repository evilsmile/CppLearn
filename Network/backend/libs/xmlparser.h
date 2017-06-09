#ifndef __XML_PARSER_H__
#define __XML_PARSER_H__

#include <tinyxml2.h>
#include <string>

class XmlParser {
	public:
		XmlParser();
		int loadFile(const std::string& file);
		int getValueByPath(const std::string& path, std::string& value);
        std::string getValueByPath(const std::string& path);
	
		int loadData(const std::string& data);

	private:
		tinyxml2::XMLDocument _doc;

};

#endif
