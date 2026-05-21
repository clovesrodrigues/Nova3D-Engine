#include <fstream>
#include <iostream>
#include <string>

int main(){
    std::ifstream f("assets/models/test.obj");
    if(!f){ std::cerr << "Validator failed: unable to open assets/models/test.obj\n"; return 1; }
    std::string line;
    int faceCount=0;
    while(std::getline(f,line)){ if(line.rfind("f ",0)==0) ++faceCount; }
    if(faceCount < 1){ std::cerr << "Validator failed: OBJ without faces\n"; return 1; }
    std::cout << "Validator OK: OBJ faces=" << faceCount << "\n";
    return 0;
}
