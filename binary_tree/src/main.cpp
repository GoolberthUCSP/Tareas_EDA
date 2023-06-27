#include <iostream>
#include <fstream>
#include <thread>
#include <random>
#include <ctime>
#include <chrono>
#include <vector>
#include "binary_tree.h"
#define N_POINTS 10000
#define MAX_INT 2000000000 //Máximo entero
#define BLOCK_SIZE 10000 //Tamaño del bloque de lectura

#define cout std::cout

int main(){
    srand(time(NULL));
    bin_tree tree;
    
    // //Paralelo
    // auto start = std::chrono::high_resolution_clock::now();
    // int n_threads = thread::hardware_concurrency();
    // vector<thread> threads;
    // for (int i = 0; i < n_threads; i++){
    //     threads.push_back(thread([&tree, n_threads](){
    //         for (int i = 0; i < N_POINTS/n_threads; i++){
    //             //Insertar o buscar aleatoriamente
    //             if (rand() % 2 == 0){
    //                 //Insertar
    //                 tree.insert(rand() % MAX_INT);
    //             }else{
    //                 //Buscar
    //                 tree.search(rand() % MAX_INT);
    //             }
    //         }
    //     }));
    // }
    // for (auto& th : threads){
    //     th.join();
    // }
    // auto end = std::chrono::high_resolution_clock::now();
    // cout << "PARALELO::Tiempo de ejecución: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " ms" << endl;


    //Secuencial
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < N_POINTS; i++){
        //Insertar o buscar aleatoriamente
        if (rand() % 2 == 0){
            //Insertar
            tree.insert(rand() % MAX_INT);
        }else{
            //Buscar
            tree.search(rand() % MAX_INT);
        }
    }
    auto end = std::chrono::high_resolution_clock::now();
    cout << "SECUENCIAL::Tiempo de ejecución: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " ms" << endl;


}