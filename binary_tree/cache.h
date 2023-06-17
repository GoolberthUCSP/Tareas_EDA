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
    void delete_first(){
        if (this->head != NULL){
            node *temp = this->head;
            this->head = this->head->next;
            delete temp;
            this->size--;
        }
    }
    void kick_back(int key){
        //Si el elemento está en la cola, no se hace nada
        //Si no, se mueve el elemento a la cola
        if (this->tail->key != key){
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
    }
};

struct cache
{
    //map <valor, indice> de los valores almacenados en la cache
    //Controlador de la existencia de los elementos en la cache
    unordered_map<int, int> m;
    //Controlador del orden de los elementos en la cache
    linkedList l;
    int max_capacity;
    int size;
    //Constructor
    cache(int max_capacity= CACHE_SIZE){
        this->max_capacity = max_capacity;
        this->size = 0;
    }
    void insert(int key, int index){
        //Si el elemento no está en la cache
        if (this->m.find(key) == this->m.end()){
            //Si la cache está llena
            if (this->size == this->max_capacity){
                //Se elimina el primer elemento de la lista
                this->l.delete_first();
                //Se elimina el primer elemento del mapa
                this->m.erase(this->m.find(this->l.head->key));
                //Se inserta el nuevo elemento en la lista
                this->l.push_back(key);
                //Se inserta el nuevo elemento en el mapa
                this->m.insert(pair<int, int>(key, index));
            }
            //Si la cache no está llena
            else{
                //Se inserta el nuevo elemento en la lista
                this->l.push_back(key);
                //Se inserta el nuevo elemento en el mapa
                this->m.insert(pair<int, int>(key, index));
                this->size++;
            }
        }
        //Si el elemento está en la cache
        else{
            //Se mueve el elemento a la cola
            this->l.kick_back(key);
        }
    }
    int search(int key){
        //Si el elemento está en la cache
        if (this->m.find(key) != this->m.end()){
            //Se mueve el elemento a la cola
            this->l.kick_back(key);
            return this->m.find(key)->second;
        }
        //Si el elemento no está en la cache
        else{
            return -1;
        }
    }
    
};

#endif //CACHE_H