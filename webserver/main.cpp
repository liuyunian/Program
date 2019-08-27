#include "c_server.h"

int main(){
    Server server(9080);
    server.run();

    return 0;
}