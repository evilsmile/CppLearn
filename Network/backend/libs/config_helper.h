#ifndef __CONFIG_HELPER__
#define __CONFIG_HELPER__

#include <memory>
#include <common.h>
#include <config.h>
#include "xmlparser.h"

class Config {
    public:
        int init(int argc, char *argv[]);
        std::string get_str(const std::string& item);
        int get_int(const std::string& item);

    private:
        XmlParser _parser;
};


#endif
