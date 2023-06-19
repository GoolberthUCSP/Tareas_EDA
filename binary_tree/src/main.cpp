#include <iostream>
//#include <fstream>
#include <thread>
#include <random>
#include <ctime>
#include "binary_tree.h"
//#define INPUT "points.bin" //Para insertar los datos en el árbol
#define N_POINTS 50000000
#define MAX_INT 2000000000 //Máximo entero

using namespace std;

int main(){
    srand(time(NULL));
    bin_tree tree;
    int rand_num;

    for (int i=0; i<10; i++){
        rand_num = rand()%MAX_INT + 1;
        tree.search(rand_num);
    } 

    //Para insertar los datos en el árbol
    // for (int i=0; i<N_POINTS; i++){
    //     int data;
    //     file.seekg(sizeof(int)*i, ios::beg);
    //     file.read((char*)&data, sizeof(int));
    //     tree.insert(data);
    // }
}