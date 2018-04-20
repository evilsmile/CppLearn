#include <iostream>
#include <string>
#include <fstream>
#include <inttypes.h>
#include <cstdlib>
#include <iomanip>

static uint64_t total_len = 0;

void handleLine(const std::string& line)
{
    size_t start = line.find('-');
    size_t end = line.find(' ', start);

    std::string startOff = line.substr(0, start);
    std::string endOff = line.substr(start+1, end-1);

    char *endPtr = NULL;

    uint64_t start_off = strtoul(startOff.c_str(), &endPtr, 16);
    uint64_t end_off = strtoul(endOff.c_str(), &endPtr, 16);
    uint64_t len = end_off - start_off;
    total_len += len;

    double ks = (double)len/1024.00;

    std::cout << line << " ***** [ " << std::dec << len << " ] "
              << "[ " << std::setiosflags(std::ios::fixed) << std::setprecision(2) << ks << "k ] "
              << "[ 0x" << std::hex << len << " ] *****"  << std::endl;
}

int main(int argc, char* argv[])
{
    std::string line;

    std::string pid = argv[1];

    std::string proc_path = "maps";
    //std::string proc_path = "/proc/"+pid+"/maps";

    std::ifstream in(proc_path.c_str(), std::ifstream::in);

    if (!in)
    {
        std::cerr << "open '" << proc_path << "' failed!" << std::endl;
        return -1;
    }

    while (getline(in, line))
    {
        handleLine(line);
    }

    std::cout << "=====> totalLen: " 
              << std::dec << total_len << ":" << total_len/1024 << "K:0x" << std::hex << total_len 
              << "<===========" << std::endl;

    return 0;
}
