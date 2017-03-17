#include <iostream>
#include <string>
#include <sstream>

int convert(const std::string& bmp_file_name, const std::string& jbg_file_name)
{
    std::ifstream fin;
    fin.open(bmp_file_name, std::ios::binary);

    if (!fin.is_open()) {
        log_error("open %s failed.", bmp_file_name.c_str());
        return -1;
    }                                                                                                       
    std::ostringstream tmptr;
    tmptr << fin.rdbuf();
    std::string bmp_data = tmptr.str();

    // ....

    std::ofstream fout(jbg_file_name);
    fout << jbg_data;

    fin.close();
    fout.close();

    return 0;
}

int main(int argc, char* argv[])
{

	return 0;
}

