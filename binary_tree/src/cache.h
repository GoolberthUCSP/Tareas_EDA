#ifndef CACHE_H
#define CACHE_H

#include <iostream>
#include <unordered_map>
const int CACHE_SIZE = 100000; //Tamaño de la caché

using namespace std;

struct node
{
    int key;
    node *next;
    node(int key){
        this->key = key;
        this->next = NULL;
    }
};

struct linkedList
{
    //Solo sirve para saber el último elemento de la lista
    node *head;
    node *tail;
    int size;
    //Constructor
    linkedList(){
        this->head = NULL;
        this->tail = NULL;
        this->size = 0;
    }
    void push_back(int key){
        node *new_node = new node(key);
        if (this->head == NULL){
            this->head = new_node;
            this->tail = new_node;
        }
        else{
            this->tail->next = new_node;
            this->tail = new_node;
        }
        this->size++;
    }
    void pop_front(){
        if (this->head != NULL){
            node *temp = this->head;
            this->head = this->head->next;
            delete temp;
            this->size--;
        }
    }
    void kick_back(int key){
        //Si el elemento está en la cola, no se hace nada
        if (this->tail->key == key) return;
        //Si no, se mueve el elemento a la cola
        node *temp = this->head;
        while (temp->next->key != key){
            temp = temp->next;
        }
        node *temp2 = temp->next;
        temp->next = temp->next->next;
        temp2->next = NULL;
        this->tail->next = temp2;
        this->tail = temp2;
    }
};

struct Cache
{
    //map <valor, indice> de los valores almacenados en la cache
    //Controlador de la existencia de los elementos en la cache
    unordered_map<int, int> map_cache;
    //Controlador del orden de los elementos en la cache
    linkedList list_cache;
    int max_capacity;
    int size;
    //Constructor
    Cache(int max_capacity= CACHE_SIZE){
        this->max_capacity = max_capacity;
        this->size = 0;
    }
    void insert(int key, int index){
        //Si el elemento no está en la cache
        if (this->map_cache.find(key) == this->map_cache.end()){
            //Si la cache está llena
            if (this->size == this->max_capacity){
                //Se obtiene la clave del primer elemento de la lista
                int to_delete = this->list_cache.head->key;
                //Se elimina el primer elemento de la lista
                this->list_cache.pop_front();
                //Se elimina el elemento que fue eliminado de la lista del mapa
                this->map_cache.erase(to_delete);
                //Se inserta el nuevo elemento en la lista
                this->list_cache.push_back(key);
                //Se inserta el nuevo elemento en el mapa
                this->map_cache.insert(pair<int, int>(key, index));
            }
            //Si la cache no está llena
            else{
                //Se inserta el nuevo elemento en la lista
                this->list_cache.push_back(key);
                //Se inserta el nuevo elemento en el mapa
                this->map_cache.insert(pair<int, int>(key, index));
                this->size++;
            }
        }
        //Si el elemento está en la cache
        else{
            //Se mueve el elemento a la cola
            this->list_cache.kick_back(key);
        }
    }
    int search(int key){
        //Si el elemento está en la cache
        if (this->map_cache.find(key) != this->map_cache.end()){
            //Se mueve el elemento a la cola
            this->list_cache.kick_back(key);
            return this->map_cache.find(key)->second;
        }
        //Si el elemento no está en la cache
        else{
            return -1;
        }
    }
};

#endif //CACHE_H