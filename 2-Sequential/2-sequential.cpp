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

string calculate(string rules[], string last , int size){

    for (int i = 0; i<size; i++){

        if (rules[i].compare(last)==0){

            return rules[i+1];
        }
    }
    cout <<"--|"<< last <<"|"<< endl;
    return "";
}
int mod (int n, int m){
    if (n<0){
        return m+n;
    }
    else if (n>=m){
        return n-m;
    }
    else{
        return n;
    }
}

int main (int argc, char** argv) {

    if (argc < 2){

        exit(1);
    };

    char* file1 = argv[1];
    char* file2 = argv[2];
    int t = atoi(argv[3]);

    cout << file1 <<  endl;
    ifstream ruleFile;
    ruleFile.open(file1);
    if (!ruleFile) {
        cerr << "Unable to open file1";
        exit(1);
    }
    string rules[7000];
    int i = 0;
    string str;
    string y;

    while (ruleFile >> str >> y) {
        rules[i]=str;
        rules[i+1] = y;
        i=i+2;

    }

    ruleFile.close();
    
    ifstream inpFile;
    inpFile.open(file2);
    if (!inpFile) {
        cerr << "Unable to open file2";
        exit(1);
    }
    inpFile >> y;

    int width = stoi(y);
    cout << width << typeid(width).name()<< width << endl;
    string stateT[width*width];
    i=0;
    while (inpFile >> str) {
        stateT[i]=str;
        i++;
        cout << str << endl;

    }
    cout << "-----------" << endl;

    inpFile.close();

    for (int gen = 0; gen<t; gen++) {
        string nextState[width][width];
        for (int i = 0; i <width; i++) {
            for (int j = 0; j <width; j++) {
                string d;

                d += stateT[mod(i-1,width)][mod(j-1,width)];
                d += stateT[mod(i-1,width)][j];
                d += stateT[mod(i-1,width)][mod(j+1,width)];
                d += stateT[i][mod(j-1,width)];
                d += stateT[i][j];
                d += stateT[i][mod(j+1,width)];
                d += stateT[mod(i+1,width)][mod(j-1,width)];
                d += stateT[mod(i+1,width)][j];
                d += stateT[mod(i+1,width)][mod(j+1,width)];


                nextState[i][j] += calculate(rules, d, 1024);

            }

        }

        for (int i = 0; i <width; i++) {
            string c;
            for (int j = 0; j < width; j++) {
                c += nextState[i][j];
            }
            cout <<c<<endl;
            stateT[i] = c;
        }
        cout <<"-------------"<<endl;

    }

}
