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
    for (int i = 0; i<1024; i=i+2){

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

string calculate(string *rules, int width,int height, char* lastState, char* up, char* down){
    string nextState;

    for (int i = 0; i <height; i++) {
        for (int j = 0; j <width; j++) {

            string d;
            if(i==0){
                d += up[mod(j-1,width)];
                d += up[j];
                d += up[mod(j+1,width)];
            }
            else{
                d += lastState[(i-1)*width+mod(j-1,width)];
                d += lastState[(i-1)*width+mod(j,width)];
                d += lastState[(i-1)*width+mod(j+1,width)];
            }

            d += lastState[i*width+mod(j-1,width)];
            d += lastState[i*width+j];
            d += lastState[i*width+mod(j+1,width)];

            if(i==height-1){
                d += down[mod(j-1,width)];
                d += down[j];
                d += down[mod(j+1,width)];
            }
            else{
                d += lastState[(i+1)*width+mod(j-1,width)];
                d += lastState[(i+1)*width+mod(j-1,width)];
                d += lastState[(i+1)*width+mod(j-1,width)];
            }

            char l = lookup(rules, d);
            if(l == '1' || l =='0') {
                nextState += l;
            }
            else{
                nextState += '0';
            }
        }

    }
    return nextState;


}



int main (int argc, char** argv) {
    int comm_sz,my_rank, t,local_size, width,rule_size;
    char* rules=(char*)malloc(5120*sizeof(char));
    char* stateT;
    double start;
    rule_size=512*10;
    int MPI_SEND_TO_UP = 0;
    int MPI_SEND_TO_DOWN = 1;

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
        cout << "start " << comm_sz<<" , " << t << endl;


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


        stateT = (char*)malloc(width*width*sizeof(char));
        {
            int r = 0;
            while (inpFile >> str) {
                for(int j=0;j<str.length();j++){
                    stateT[r]=str[j];
                    r++;
                }
            }
        }

        inpFile.close();
        local_size= (width*width)/comm_sz;
        if((width*width)%comm_sz!=0){
            cout <<width*width<< "inputsize not divisible by "<<comm_sz << " processes";
            exit(1);
        }

        MPI_Bcast(&local_size, sizeof(int), MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Bcast(rules,rule_size, MPI_BYTE, 0, MPI_COMM_WORLD);
        MPI_Bcast(&t, sizeof(int), MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Bcast(&width, sizeof(int), MPI_INT, 0, MPI_COMM_WORLD);



    }
    else{


        MPI_Bcast(&local_size, sizeof(int), MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Bcast(rules, rule_size, MPI_BYTE, 0, MPI_COMM_WORLD);
        MPI_Bcast(&t, sizeof(int), MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Bcast(&width, sizeof(int), MPI_INT, 0, MPI_COMM_WORLD);



    }
    MPI_Barrier(MPI_COMM_WORLD);




    //Translating rules back to strings for easier comparison
    string rulesAsString[5120];

    for (int i=0; i<512; i=i+2){
        string temp;
        string y;
        for (int j=0; j<9;j++){
            temp+=rules[10*i+j];

        }
        y+=rules[10*i+9];
        rulesAsString[i]= temp;
        rulesAsString[i+1]=y;
    }



    char* local_string = (char*) malloc(local_size*sizeof(char)+1   );


    if(my_rank==0) {
        MPI_Scatter(stateT, local_size, MPI_CHAR, local_string, local_size, MPI_BYTE, 0, MPI_COMM_WORLD);
    }
    else{
        MPI_Scatter(stateT, local_size, MPI_CHAR, local_string, local_size, MPI_BYTE, 0, MPI_COMM_WORLD);
    }

    int height= local_size/width;



    for(int i=0; i < t; i++){
        char* toup=(char*)malloc(width*sizeof(char));
        char* todown=(char*)malloc(width*sizeof(char));
        string tempUP;
        string tempDOWN;
        for(int j=0; j<width; j++){
            tempUP+=local_string[j];
            tempDOWN+=local_string[(local_size-1)-((width-1)-j)] ;

        }
        strcpy(toup,tempUP.c_str());
        strcpy(todown,tempDOWN.c_str());

        char* up=(char*)malloc(width* sizeof(char));
        char* down=(char*)malloc(width* sizeof(char));

        MPI_Send(toup,width, MPI_BYTE,mod(my_rank-1,comm_sz),MPI_SEND_TO_UP, MPI_COMM_WORLD);
        MPI_Send(todown,width, MPI_BYTE,mod(my_rank+1,comm_sz),MPI_SEND_TO_DOWN, MPI_COMM_WORLD);

        MPI_Recv(down,width, MPI_BYTE,mod(my_rank+1,comm_sz),MPI_SEND_TO_UP, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(up,width, MPI_BYTE,mod(my_rank-1,comm_sz),MPI_SEND_TO_DOWN, MPI_COMM_WORLD, MPI_STATUS_IGNORE);


        string next =calculate(rulesAsString, width, height, local_string,up,down);
        strcpy(local_string, next.c_str());
    }
    MPI_Gather(local_string, local_size, MPI_BYTE, stateT, local_size, MPI_BYTE, 0, MPI_COMM_WORLD);

    if(my_rank==0){
        cout << MPI_Wtime()-start << endl;

    }

    MPI_Finalize();
}
