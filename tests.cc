#include "wrapperfun.h"
#include <iostream>


using namespace std;
using namespace util;

int main(int argc, char *argv[])
{
    /********************************/
    errno = 10;
    unix_error("test unix error");
    errno = 0;



    /************************************/
    int listenfd = Open_listenfd(8080);
    cout << "test listenfd : " << listenfd << endl;


    return 0;

}
