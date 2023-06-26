#ifndef BIN_TREE_H
#define BIN_TREE_H

#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <mutex>
#include "cache.h"

#define FILENAME "data.bin"
#define MAX_SIZE 100000000 //Tamaño máximo de enteros a almacenar 100M
#define FLAGS (std::ios::in | std::ios::binary | std::ios::out)
#define BLOCK_SIZE 10000 //Tamaño del bloque de lectura y escritura 10K
#define cout std::cout
using namespace std;

/*
Se implementará un árbol binario de búsqueda para almacenar números enteros
El árbol será persistente, por lo tanto se usará un fichero .bin para almacenar los datos
*/
class bin_tree{
private:
    fstream file;
    //Memoria caché para los valores más buscados
    Cache cache;
    //Bloquear el acceso de lectura y escritura al fichero
    mutex rd_mtx, wr_mtx;
public:
    bin_tree(){
        //Fichero de entrada y salida
        file.open(FILENAME, FLAGS);
        if (file.peek() == std::ifstream::traits_type::eof()){
            //Crear fichero
            file.close();
            file.open(FILENAME, ios::out | ios::binary);
            file.close();
            //Inicializar fichero con 100M de ceros
            file.open(FILENAME, FLAGS);
            int zero[BLOCK_SIZE]{0};
            for (int i = 0; i < MAX_SIZE/BLOCK_SIZE; i++){
                file.write((char*)zero, sizeof(int)*BLOCK_SIZE);
            }
            cout << "Fichero inicializado" << endl;
        }
    }
    ~bin_tree(){
        file.close();
    }
    
    bool insert(int target, int index=0){
        //Insertar un valor en el árbol
        if (search(target) != -1){ //Si el valor ya está en el árbol
            return false;
        }
        if (index >= MAX_SIZE) return false;
        //Insertar en el fichero
        int rd_int= read(index);
        //Si se llega a un nodo inexistente = 0
        if (rd_int == 0){
            cache.insert(target, index);
            write(target, index);
            return true;
        }
        //Insertar en el subárbol izquierdo
        if (target < rd_int){
            return insert(target, 2*index+1);
        }
        //Insertar en el subárbol derecho
        else{
            return insert(target, 2*index+2);
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
            //Si se encontró, añadir a la caché
            if (index != -1){
                cache.insert(target, index);
            }
        }
        return index;
    }

    int search_in_file(int target, int index){
        //Retorna el índice del registro que contiene el valor target
        if (index >= MAX_SIZE) return -1;
        int rd_int= read(index);
        //Si se llega a un nodo inexistente = 0
        if (rd_int == 0){
            return -1;
        }
        //Si se llega al valor buscado
        if (rd_int == target){
            return index;
        }
        //Buscar en el subárbol izquierdo
        if (target < rd_int){
            return search_in_file(target, 2*index+1);
        }
        //Buscar en el subárbol derecho
        else{
            return search_in_file(target, 2*index+2);
        }
    }

    void read_list(int min, int max, int* arr){
        //Leer los valores del árbol que están en el rango [min, max>
        file.seekg(sizeof(int)*min, ios::beg);
        file.read((char*)arr, sizeof(int)*(max-min));
    }
    void read_by_range_in_file(int min, int max, int index, int* arr){
        //Leer los valores del árbol que están en el rango [min, max>
        if (index >= MAX_SIZE) return;
        int rd_int= read(index);
        //Si se llega a un nodo inexistente = 0
        if (rd_int == 0){
            return;
        }
        //Si se llega al valor buscado
        if (rd_int >= min && rd_int < max){
            arr[index] = rd_int;
        }
        read_by_range_in_file(min, max, 2*index+1, arr);
        read_by_range_in_file(min, max, 2*index+2, arr);
    }

    int read(int index){
        //Leer el valor del registro index en el fichero
        lock_guard<mutex> lock(rd_mtx);
        int rd_int;
        file.seekg(sizeof(int)*index, ios::beg);
        file.read((char*)&rd_int, sizeof(int));
        return rd_int;
    }

    void write(int &target, int index){
        //Escribir el valor del entero target en el registro index del fichero
        lock_guard<mutex> lock(wr_mtx);
        file.seekp(sizeof(int)*index, ios::beg);
        file.write((char*)&target, sizeof(int));
    }
};

#endif //BIN_TREE_H