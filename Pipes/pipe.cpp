#include<cstdlib>
#include<cstdio>
#include<array>
#include<string>
#include<iostream>

using namespace std;

void pipecommand(string strcmd){

    std::array<char, 128> buffer;
    FILE *pipe = popen(strcmd.c_str(), "r");
    if (!pipe){
        cout << "Error opening pipe" << endl;
        return;
    }

    int c = 0;
    while (fgets(buffer.data(), buffer.size(), pipe) != NULL) {
        cout << buffer.data();
        c++;
    }
    pclose(pipe);
}

int main(int argc, char *argv[]){

    string strcmd = "";
    while (1)
    {
        std::cout << "Enter command: ";
        std::getline(std::cin, strcmd);
        pipecommand(strcmd);
    }
    

    return 0;
}