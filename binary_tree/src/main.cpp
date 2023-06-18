#include <iostream>
#include <fstream>
#include "binary_tree.h"
#define INPUT "points.bin"
#define N_POINTS 50000000

using namespace std;

int main(){
    bin_tree tree;
    fstream file;
    file.open(INPUT, ios::binary | ios::in);
    for (int i=0; i<N_POINTS; i++){
        int data;
        file.seekg(sizeof(int)*i, ios::beg);
        file.read((char*)&data, sizeof(int));
        tree.insert(data);
    }
}