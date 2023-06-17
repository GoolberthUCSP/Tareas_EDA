#ifndef BIN_TREE_H
#define BIN_TREE_H

#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <string>
#include <cassert>
#include "cache.h"

#define FILENAME "data.bin"
#define SIZE 100000000 //Tamaño máximo de enteros a almacenar

using namespace std;

/*
Se implementará un árbol binario de búsqueda para almacenar los datos de los registros
El árbol debe ser persistente, por lo tanto se usará un fichero .bin para almacenar los datos

*/
class bin_tree{
    fstream file;
    //Puntero que recorrerá el fichero
    int *rd_ptr; //Puntero de lectura
    cache c;
public:
    bin_tree(){
        //Fichero de entrada y salida
        file.open(FILENAME, ios::binary | ios::in | ios::out);
        //Si no existe el fichero, se crea y se inicializa con SIZE ceros
        if (!file.is_open()){
            file.open(FILENAME, ios::binary | ios::out);
            int *ptr = new int{0};
            *ptr = 0;
            for (int i=0; i<SIZE; i++){
                file.write((char*)ptr, sizeof(int));
            }
            file.close();
            file.open(FILENAME, ios::binary | ios::in | ios::out);
        }
        //Puntero inicializado al inicio del fichero
        rd_ptr = new int{0};
    }
    ~bin_tree(){
        file.close();
    }
    //left=2*N+1 right=2*N+2
    int search(int target){
        //Buscar en la caché
        int index = c.search(target);
        if (index == -1){
            index= search_in_file(target, 0);
        }
        return index;
    }

    int search_in_file(int target, int index){
        //Retorna el índice del registro que contiene el valor target
        assert(index >= 0 && index < SIZE);
        read(index);
        //Si se llega a un nodo inexistente
        if (!(*rd_ptr)){ 
            return -1;
        }
        //Si se llega al valor buscado
        if (target == *rd_ptr){
            return index;
        }
        
        if (target < *rd_ptr){
            return search_in_file(target, 2*index+1);
        }
        else{
            return search_in_file(target, 2*index+2);
        }
    }

    void read(int index){
        //Mover el puntero de lectura al registro index
        file.seekg(sizeof(int)*index, ios::beg);
        //Leer el valor del registro
        file.read((char*)rd_ptr, sizeof(int));
    }
    void write(int index, int value){
        //Mover el puntero de escritura al registro index
        file.seekp(sizeof(int)*index, ios::beg);
        //Escribir el valor en el registro
        file.write((char*)&value, sizeof(int));
    }
};


#endif //BIN_TREE_H