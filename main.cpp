#include <iostream>
#include <fstream>
#include "YAMLParser.h"


inline YAMLEncoding load(std::istream& is)
{
    EventStream st = is;
    auto tokens = YAMLEvent(st).get();
    return YAMLParser(tokens.begin()).get();
}

int main()
{
    std::ifstream ifs("test.yaml");
    auto yml = ::load(ifs);
    std::cout << yml.get<YAMLEncoding::dataList>()[0].get<std::string>();
    system("pause");
}