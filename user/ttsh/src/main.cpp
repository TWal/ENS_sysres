#include <iostream>
#include "Commands.h"
#include "Scanner.h"

using namespace std;

int main() {
    auto& input = cin;
    while(true) {
        printf("$ ");
        Scanner scan(&input,"test"); // load scanner
        Command* cmd = nullptr;
        yy::parser parser(scan,cmd); //load parser
        parser.parse(); //parse
        
        if(cmd) cmd->run();

        delete cmd;
    }

    return 0;
}
