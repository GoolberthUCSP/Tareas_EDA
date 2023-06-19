#include <iostream>
#include <fstream>
#include <thread>
#include <random>
#include <ctime>
#include "binary_tree.h"
#define INPUT "points.bin" //Para insertar los datos en el árbol
#define N_POINTS 50000000
#define MAX_INT 2000000000 //Máximo entero

int main(){

    bin_tree tree;
    
    // srand(time(NULL));
    // int rand_num, index;
    // int range = 50;
    // int *arr= tree.read_by_range(0, range);
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
    int data[N_POINTS]{0};
    file.seekg(0, ios::beg);
    file.read((char*)data, sizeof(int)*N_POINTS);
    for (int i=0; i<N_POINTS; i++){
        if (tree.insert(data[i])) cout << "Insertado " << data[i] << endl;
    }
    file.close();
}