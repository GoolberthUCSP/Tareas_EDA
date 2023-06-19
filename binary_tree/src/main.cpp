#include <iostream>
#include <fstream>
#include <thread>
#include <random>
#include <ctime>
#include "binary_tree.h"
#define INPUT "points.bin" //Para insertar los datos en el árbol
#define N_POINTS 50000000
#define MAX_INT 2000000000 //Máximo entero
#define BLOCK_SIZE 10000 //Tamaño del bloque de lectura

#define cout std::cout

int main(){

    bin_tree tree;
    
    // srand(time(NULL));
    // int rand_num, index;
    // int range = 10;
    // int *arr= new int[range]{0};
    // tree.read_by_range(0, range, arr);
    // for (int i=0; i<range; i++){
    //     cout << arr[i] << ' ';
    // }
    // cout << endl;
    // for (int i=0; i<N_POINTS/100; i++){
    //     rand_num = rand()%MAX_INT + 1;
    //     index= tree.search(rand_num);
    //     if (index != -1){
    //         cout << "El número " << rand_num << "\tse encuentra en el índice " << index << endl;
    //     }
    // }

    fstream file;
    file.open(INPUT, ios::binary | ios::in);
    if(!file.is_open()){
        cout << "Error al abrir el fichero" << endl;
    }
    //Insertar los datos en el árbol
    int data[BLOCK_SIZE];
    for (int i=0; i<N_POINTS/BLOCK_SIZE; i++){
        file.seekg(i*BLOCK_SIZE*sizeof(int), ios::beg);
        file.read((char*)data, sizeof(int)*BLOCK_SIZE);
        for (int j=0; j<BLOCK_SIZE; j++){
            tree.insert(data[j]);
        }
        cout << data[0] << ' ' << data[BLOCK_SIZE-1] << endl;
    }
    file.close();
}