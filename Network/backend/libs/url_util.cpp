#include "url_util.h"

static unsigned char to_hex(unsigned char x)
{
    return x > 9? x + 55 : x + 48;
}

static unsigned char from_hex(unsigned char x)
{
    unsigned char y;
    if (x >= 'A' && x <= 'Z') {
        y = x - 'A' + 10;
    } else if (x >= 'a' && x <= 'z') {
        y = x - 'a' + 10;
    } else if (x >= '0' && x <= '9') {
        y = x - '0';
    } else {
        y = -1;
    }

    return y;
}

std::string url_encode(const std::string& str)
{
    std::string tmp;
    size_t length = str.length();
    for (size_t i = 0; i < length; ++i) {
        if (isalnum((unsigned char)str[i])
                || str[i] == '-'
                || str[i] == '_'
                || str[i] == '.'
                || str[i] == '~') {
            tmp += str[i];
        } else if (str[i] == ' ') {
            tmp += "+";
        } else {
            tmp += "%";
            tmp += to_hex((unsigned char)str[i] >> 4);
            tmp += to_hex((unsigned char)str[i] % 16);
        }
    }

    return tmp;
}


std::string url_decode(const std::string& str)
{
    std::string tmp;

    size_t length = str.length();
    for (size_t i = 0; i < length; ++i) {
        if (str[i] == '+') {
            tmp += ' ';
        } else if (str[i] != '%') {
            tmp += str[i];
        } else {
            unsigned char high = from_hex((unsigned char)str[++i]);
            unsigned char low = from_hex((unsigned char)str[++i]);
            tmp += high *16 + low;
        }
    }

    return tmp;
}
