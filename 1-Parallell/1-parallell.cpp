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
#include <mpi.h>

using namespace std;

char lookup(string *rules, string last){
    for (int i = 0; i<18; i=i+2){

        if (rules[i].compare(last)==0){

            return rules[i+1][0];
        }
    }
    return ' ';
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
string calculate(string *rules, int width, char* lastState, char first, char last){
    string nextState;
    for (int i = 0; i <width; i++) {
        string d;
        if(i == 0){
            d+=first;
        }
        else{
            d += lastState[i-1];
        }
        d +=lastState[i];

        if(i == width-1){
            d+=last;
        }
        else{
            d += lastState[i+1];
        }

        nextState += lookup(rules, d);
    }
    return nextState;
}




int main (int argc, char** argv) {
    int comm_sz,my_rank, t,local_size, width;
    char* rules=(char*)malloc(32*sizeof(char));
    char* stateT;
    double start, end;
    int MPI_SEND_TO_LEFT = 0;
    int MPI_SEND_TO_RIGHT = 1;

    //cout << "start" << endl;

    MPI_Init(NULL,NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    if (my_rank == 0) {
        start = MPI_Wtime();
        if (argc < 2) {

            exit(1);
        };

        char *file1 = argv[1];
        char *file2 = argv[2];
        t = atoi(argv[3]);

        
        ifstream ruleFile;
        ruleFile.open(file1);
        if (!ruleFile) {
            cerr << "Unable to open file1";
            MPI_Finalize();
            exit(1);
        }

        int i = 0;
        string str;
        string y;
        //cout << "wtf" << endl;
        while (ruleFile >> str >> y) {
            for(int j=0;j<str.length();j++){
                rules[i]=str[j];
                i++;
            }

            rules[i]=y[0];
            i++;
        }

        ruleFile.close();


        ifstream inpFile;
        inpFile.open(file2);
        if (!inpFile) {
            cerr << "Unable to open file2";
            MPI_Finalize();
            exit(1);
        }
        inpFile >> y;
        width = stoi(y);
        inpFile >> str;
        stateT = (char*)malloc(sizeof(char)*width);

        for (int i = 0; i < str.length(); i++){
            stateT[i] = str[i];
        }
        


        inpFile.close();

        local_size= width/comm_sz;
        MPI_Bcast(&local_size, sizeof(int), MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Bcast(rules,32, MPI_BYTE, 0, MPI_COMM_WORLD);
        MPI_Bcast(&t, sizeof(int), MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Bcast(&local_size, sizeof(int), MPI_INT, 0, MPI_COMM_WORLD);



    }
    else{
        MPI_Bcast(&local_size, sizeof(int), MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Bcast(rules, 32, MPI_BYTE, 0, MPI_COMM_WORLD);
        MPI_Bcast(&t, sizeof(int), MPI_INT, 0, MPI_COMM_WORLD);



    }
    MPI_Barrier(MPI_COMM_WORLD);


    char* local_string = (char*) malloc(local_size*sizeof(char));

    //Translating rules back to strings for easier comparison
    string rulesAsString[32];
    int arraycount=0;
    for (int i=0; i<32; i=i+4){
        string temp;
        string y;
        temp+=rules[i];
        temp+=rules[i+1];
        temp+=rules[i+2];
        y+=rules[i+3];
        //cout <<"---"<< temp<< endl;
        rulesAsString[arraycount]= temp;
        rulesAsString[arraycount+1]=y;
        arraycount=arraycount+2;
    }




    if(my_rank == 0){
        MPI_Scatter(stateT,local_size,MPI_CHAR,local_string,local_size,MPI_BYTE,0,MPI_COMM_WORLD);
    }
    else{
        MPI_Scatter(stateT,local_size,MPI_CHAR,local_string,local_size,MPI_BYTE,0,MPI_COMM_WORLD);
    }

    for(int i=0; i < t; i++){
        char toleft =local_string[0];
        char toright=local_string[local_size-1];
        char leftmost;
        char rightmost;
        MPI_Send(&toleft,1, MPI_CHAR,mod(my_rank-1,comm_sz),MPI_SEND_TO_LEFT, MPI_COMM_WORLD);
        MPI_Send(&toright,1, MPI_CHAR,mod(my_rank+1,comm_sz),MPI_SEND_TO_RIGHT, MPI_COMM_WORLD);
        MPI_Recv(&rightmost,1, MPI_CHAR,mod(my_rank+1,comm_sz),MPI_SEND_TO_LEFT, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&leftmost,1, MPI_CHAR,mod(my_rank-1,comm_sz),MPI_SEND_TO_RIGHT, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        //cout << my_rank << ": " << leftmost << endl;
        string step = calculate(rulesAsString, local_size, local_string,leftmost,rightmost);

        strcpy(local_string,step.c_str());

    }
    MPI_Gather(local_string, local_size, MPI_CHAR, stateT, local_size, MPI_CHAR, 0, MPI_COMM_WORLD);

    if(my_rank==0){
        cout << MPI_Wtime()-start << endl;
        //for (int i=0; i< width; i++){
            //cout << stateT[i];
        //}
        //cout << "" << endl;
    }

    MPI_Finalize();
}
