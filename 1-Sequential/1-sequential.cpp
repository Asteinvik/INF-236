//Copyright 2019 Andreas Steinvik

//Permission is hereby granted, free of charge, to any person obtaining a copy 
//of this software and associated documentation files (the "Software"), 
//to deal in the Software without restriction, including without limitation the 
//rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell 
//copies of the Software, and to permit persons to whom the Software is furnished 
//to do so, subject to the following conditions:

//The above copyright notice and this permission notice shall be included in all 
//copies or substantial portions of the Software.

//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, 
//EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
//BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR 
//OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
//DEALINGS IN THE SOFTWARE.



#include <iostream>
#include <stdlib.h>
#include <fstream>
using namespace std;
string calculate(string rules[], string last ){
    for (int i = 0; i<18; i=i+2){

        if (rules[i].compare(last)==0){

            return rules[i+1];
        }
    }
    return "";
}

int main (int argc, char** argv) {

    if (argc < 2){

        exit(1);
    };

    char* file1 = argv[1];
    char* file2 = argv[2];
    int t = atoi(argv[3]);


    ifstream ruleFile;
    ruleFile.open(file1);
    if (!ruleFile) {
        cerr << "Unable to open file1";
        exit(1);
    }

    string rules[50];
    int i = 0;
    string str;
    string y;
    //cout << " test" << endl;
    while (ruleFile >> str >> y) {
        cout << str << endl;
        rules[i]=str;
        rules[i+1] = y;
        i=i+2;

    }
    //cout << file1 <<  endl;
    ruleFile.close();


    ifstream inpFile;
    inpFile.open(file2);
    if (!inpFile) {
        cerr << "Unable to open file2";
        exit(1);
    }
    inpFile >> y;
    inpFile >> str;
    string stateT = str;
    int width = stoi(y);

    inpFile.close();
    cout << width << typeid(width).name()<< stateT.length() << typeid(stateT.length()).name()<< endl;
    cout << stateT << endl;
    for (int gen = 0; gen<t; gen++) {
        string nextState;
        for (int i = 0; i <stateT.length(); i++) {
            string d;
            d += stateT[(i-1) % stateT.length()];
            d +=stateT[i];
            d += stateT[(i+1) % stateT.length()];


            nextState += calculate(rules, d);
        }
        stateT = nextState;
        cout << stateT << endl;
    }

}
