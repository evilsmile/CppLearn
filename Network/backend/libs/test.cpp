#include <stringutil.h>
#include <xmlparser.h>

#include <log.h>

#include <string>
#include <vector>
#include <iostream>
#include <common.h>

bool test_splitString()
{
	strings_t splited;
	std::string test = "a/bc/b/c/";
	splitString(test, '/', splited);

	for (int i = 0; i < splited.size(); ++i) {
		std::cout << splited[i] << "|";
	}
	std::cout << std::endl;

	return true;
}

bool test_getValueByPath()
{
	XmlParser parser;
	parser.loadFile("test.xml");

	std::string value;
    if (parser.getValueByPath("///a////b", value)) {
		log_error("get xml node failed.");
		return false;
	}

	std::cout << "value: " << value << std::endl;

	return true;
}

int main()
{
	init_log("test");
//	test_splitString();
	test_getValueByPath();

	return 0;
}
