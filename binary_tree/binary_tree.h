#ifndef BIN_TREE_H
#define BIN_TREE_H

#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <string>
#define FILENAME "data.bin"
using namespace std;

/*
Se implementará un árbol binario de búsqueda para almacenar los datos de los registros
El árbol debe ser persistente, por lo tanto se usará un fichero .bin para almacenar los datos

*/
class bin_tree{
    ifstream file;
    //Puntero a la raíz del árbol= primer registro de 4 bytes almacenado en el fichero
    int *root;
    bin_tree(){
        file.open(FILENAME, ios::binary);
        if(!file.is_open()){
            cout << "Error al abrir el fichero" << endl;
        }
        file.read((char*)&root, sizeof(int*));
    }
    ~bin_tree(){
        file.close();
    }
    //root=1er registro del fichero
    //left=2*N+1 right=2*N+2
    int search(int target){
        //Retorna el índice del registro que contiene el valor target
        //Si no existe retorna -1
        //Un nodo es un registro de 4 bytes dentro del fichero

        int index = *root;
        return 0;
    }
};


#endif //BIN_TREE_H