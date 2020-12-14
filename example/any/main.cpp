#include <iostream>

#include <leaf/map/Any.h>

int main()
{
    leaf::Any any;

    // ------------------------
    std::cout << "Storing interger \n";

    any = 100;
    int int_num = any;

    std::cout << int_num << " \n" ;
    std::cout << int(any) << " \n";

    // ------------------------
    std::cout << "Storing float \n";

    any = 0.1f;
    float float_num = any;

    std::cout << float_num << " \n" ;
    std::cout << (float)any << " \n";

    // ------------------------
    std::cout << "Storing string \n";

    any = std::string("test");
    std::string str = any;

    std::cout << str << " \n" ;
    std::cout << leaf::Any::Cast<std::string>(any) << " \n";

    return 0;
}
