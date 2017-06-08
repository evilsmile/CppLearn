#include "xmlparser.h"
#include "log.h"
#include "stringutil.h"

#include <common.h>

using namespace tinyxml2;

XmlParser::XmlParser()
{
}

int XmlParser::loadFile(const std::string& file)
{
	if (_doc.LoadFile(file.c_str()) != XML_SUCCESS) {
		log_error("load xml file '%s' failed", file.c_str());
		return -1;
	}

	return 0;
}

int XmlParser::loadData(const std::string& data)
{
	if (_doc.Parse(data.c_str()) != XML_SUCCESS) {
		log_error("load xml data failed.");
		return -1;
	}

	return 0;
}

int XmlParser::getValueByPath(const std::string& path, std::string& value)
{
	strings_t path_elem;
	splitString(path, '/', path_elem);

	int ele_size = path_elem.size();
	if (ele_size < 1) {
		return 0;
	}

	XMLElement* element = NULL;

	int i = 0;
	while (path_elem[i].empty()) {
		++i;
	}

	element = _doc.FirstChildElement(path_elem[i].c_str());

	for (i = i + 1; i < ele_size && element; ++i) {
		XMLElement* element2 = NULL;
		if (!path_elem[i].empty()) {
			element2 = element->FirstChildElement(path_elem[i].c_str());
			element = element2;
		}
	}

	if (element) {
		value = element->Value();
		return 0;
	}

	return -1;
}
