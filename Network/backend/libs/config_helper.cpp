#include "config_helper.h" 

int Config::init(int argc, char* argv[])
{
    std::string standard_config_file = Const::CONFIG_FILENAME;
    if (argc > 1) {
        standard_config_file = argv[1];
    }
    int error = _parser.loadFile(standard_config_file);
    if (error) {
        return -1;
    }

    return 0;
}

std::string Config::get_str(const std::string& item)
{
    return _parser.getValueByPath(item);
}

int Config::get_int(const std::string& item)
{
    std::string value = _parser.getValueByPath(item);

    return atoi(value.c_str());
}
