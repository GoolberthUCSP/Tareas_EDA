#ifndef BIN_TREE_H
#define BIN_TREE_H

#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <string>
#include <cassert>
#include <mutex>
#include "cache.h"

#define FILENAME "data.bin"
#define MAX_SIZE 100000000 //Tamaño máximo de enteros a almacenar

using namespace std;

/*
Se implementará un árbol binario de búsqueda para almacenar números enteros
El árbol será persistente, por lo tanto se usará un fichero .bin para almacenar los datos
*/
class bin_tree{
    fstream file;
    //Puntero que recorrerá el fichero
    int *rd_ptr; //Puntero de lectura
    Cache cache;
    mutex mtx;
public:
    bin_tree(){
        //Fichero de entrada y salida
        file.open(FILENAME, ios::binary | ios::in | ios::out);
        //Si no existe el fichero, se crea y se inicializa con MAX_SIZE ceros
        if (!file.is_open()){
            file.open(FILENAME, ios::binary | ios::out);
            int zero = 0;
            for (int i=0; i<MAX_SIZE; i++){
                file.write((char*)&zero, sizeof(int));
            }
            file.close();
            file.open(FILENAME, ios::binary | ios::in | ios::out);
        }
    }
    ~bin_tree(){
        file.close();
    }
    
    bool insert(int target){
        int index = insert_in_file(target, 0);
        //Insertar en la caché, si se insertó en el fichero
        if (index != -1){
            cache.insert(target, index);
            return true;
        }
        return false;
    }

    int insert_in_file(int target, int index){
        //Retorna el índice del registro que fue insertado
        //Insertar en el fichero
        assert(index >= 0 && index < MAX_SIZE);
        read(index);
        //Si se llega a un nodo inexistente
        if (!(*rd_ptr)){ 
            *rd_ptr = target;
            write(index);
            return index;
        }
        //Si se llega al valor buscado
        if (target == *rd_ptr){
            return -1;
        }
        
        if (target < *rd_ptr){
            return insert_in_file(target, 2*index+1);
        }
        else{
            return insert_in_file(target, 2*index+2);
        }
    }

    //Buscar si un valor está en el árbol
    //Devuelve el índice del registro que contiene el valor buscado o -1 si no se encontró
    int search(int target){
        //Buscar en la caché
        int index = cache.get(target);
        //Si no se encuentra en la caché, buscar en el fichero
        if (index == -1){
            index= search_in_file(target, 0);
        }
        //Si se encontró en el fichero, insertar en la caché
        if (index != -1){
            cache.insert(target, index);
        }
        return index;
    }

    int search_in_file(int target, int index){
        //Retorna el índice del registro que contiene el valor target
        assert(index >= 0 && index < MAX_SIZE);
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
        //Leer el valor del registro index en el fichero
        mtx.lock();
        //Mover el puntero de lectura al registro index
        file.seekg(sizeof(int)*index, ios::beg);
        //Leer el valor del registro
        file.read((char*)rd_ptr, sizeof(int));
        mtx.unlock();
    }
    void write(int index){
        //Escribir el valor del registro index en el fichero
        mtx.lock();
        //Mover el puntero de escritura al registro index
        file.seekp(sizeof(int)*index, ios::beg);
        //Escribir el valor en el registro
        file.write((char*)rd_ptr, sizeof(int));
        mtx.unlock();
    }
};

#endif //BIN_TREE_H