#ifndef BIN_TREE_H
#define BIN_TREE_H

#include <iostream>
#include <vector>
#include <fstream>
#include <string>
//#include <mutex>
#include "cache.h"

#define FILENAME "data.bin"
#define MAX_SIZE 100000000 //Tamaño máximo de enteros a almacenar 100M
#define FLAGS std::ios::in | std::ios::binary | std::ios::app
#define BLOCK_SIZE 10000 //Tamaño del bloque de lectura y escritura 10K
#define cout std::cout
using namespace std;

/*
Se implementará un árbol binario de búsqueda para almacenar números enteros
El árbol será persistente, por lo tanto se usará un fichero .bin para almacenar los datos
*/
class bin_tree{
    fstream file;
    //Puntero que recorrerá el fichero
    int rd_int; //Puntero de lectura
    Cache cache;
    //mutex mtx;
public:
    bin_tree(){
        //Fichero de entrada y salida
        file.open(FILENAME, ios::in); //Verificar si el fichero existe
        //Si el fichero no existe, crearlo
        if (!file.is_open()){
            cout << "Creando fichero" << endl;
            file.open(FILENAME, ios::out);
            file.close();

            file.open(FILENAME, FLAGS);
            //Inicializar fichero
            cout << "Inicializando fichero" << endl;
            int zero[BLOCK_SIZE]{0};
            for (int i = 0; i < MAX_SIZE/BLOCK_SIZE; i++){
                file.seekp(i*BLOCK_SIZE*sizeof(int), ios::beg);
                file.write((char*)zero, sizeof(int)*BLOCK_SIZE);
            }
        }
        else {
            file.close();
            file.open(FILENAME, FLAGS);
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
        if (index < 0 || index >= MAX_SIZE) return -1;
        read(index);
        //Si se llega a un nodo inexistente
        if (!(rd_int)){ 
            rd_int = target;
            write(index);
            return index;
        }
        //Si se llega al valor buscado
        if (target == rd_int){
            return -1;
        }
        
        if (target < rd_int){
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
        if (index < 0 || index >= MAX_SIZE) return -1;
        read(index);
        //Si se llega a un nodo inexistente
        if (!(rd_int)){ 
            return -1;
        }
        //Si se llega al valor buscado
        if (target == rd_int){
            return index;
        }
        
        if (target < rd_int){
            return search_in_file(target, 2*index+1);
        }
        else{
            return search_in_file(target, 2*index+2);
        }
    }

    void read_by_range(int min, int max, int *arr){
        //Leer los valores del árbol que están en el rango [min, max]
        file.seekg(sizeof(int)*min, ios::beg);
        file.read((char*)arr, sizeof(int)*(max-min+1));
    }

    void read(int index){
        //Leer el valor del registro index en el fichero
        //mtx.lock();
        //Mover el puntero de lectura al registro index
        file.seekg(sizeof(int)*index, ios::beg);
        //Leer el valor del registro
        file.read((char*)&rd_int, sizeof(int));
        //mtx.unlock();
    }
    void write(int index){
        //Escribir el valor del registro index en el fichero
        //mtx.lock();
        //Mover el puntero de escritura al registro index
        file.seekp(sizeof(int)*index, ios::beg);
        //Escribir el valor en el registro
        file.write((char*)&rd_int, sizeof(int));
        //mtx.unlock();
    }
};

#endif //BIN_TREE_H