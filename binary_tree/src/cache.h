#ifndef CACHE_H
#define CACHE_H

#include <iostream>
#include <unordered_map>
const int CACHE_SIZE = 100000; //Tamaño de la caché

using namespace std;

struct node
{
    //key= número a buscar, value= índice en la estructura de datos
    int key, value;
    node *prev, *next;
    node(int key, int value) : key(key), value(value), prev(nullptr), next(nullptr) {}
};

struct dlist{
    node *head, *tail;
    int size;
    dlist() : head(nullptr), tail(nullptr), size(0) {}
    void push_back(int key, int value){
        node *new_node = new node(key, value);
        if (head == nullptr){
            head = new_node;
            tail = new_node;
        }
        else{
            tail->next = new_node;
            new_node->prev = tail;
            tail = new_node;
        }
        size++;
    }
    ~dlist(){
        node *temp = head;
        while (temp != nullptr){
            node *temp2 = temp;
            temp = temp->next;
            delete temp2;
        }
    }
    void pop_front(){
        if (head != nullptr){
            node *temp = head;
            head = head->next;
            if (head != nullptr) head->prev = nullptr;
            delete temp;
            size--;
        }
    }
    void kick_back(node *node_){
        //Si el elemento está en la cola, no se hace nada
        if (tail == node_) return;
        //Si no, se mueve el elemento a la cola
        if (head == node_){
            node *temp = head;
            head = head->next;
            head->prev = nullptr;
            tail->next = temp;
            temp->prev = tail;
            temp->next = nullptr;
            tail = temp;
        }
        else{
            node *left = node_->prev, 
                *right = node_->next;
            left->next = right;
            right->prev = left;
            tail->next = node_;
            node_->prev = tail;
            node_->next = nullptr;
            tail = node_;
        }
    }
};

struct Cache
{
    //key= número a buscar, value= nodo que contiene el índice
    unordered_map<int, node*> map_;
    dlist list_;
    int max_size;
    
    Cache(int max_size_= CACHE_SIZE) : max_size(max_size_) {}
    ~Cache(){
        map_.clear();
    }
    void insert(int key, int value){
        //Si el elemento está en la caché, se mueve a la cola
        if (map_.find(key) != map_.end()){
            //Mover el nodo que contiene el número a la cola
            list_.kick_back(map_[key]);
        }
        //Si no, se inserta en la caché
        else{
            //Si la caché está llena, se elimina el primer elemento
            if (list_.size == max_size){
                map_.erase(list_.head->key);
                list_.pop_front();
            }
            //Se inserta el nuevo elemento
            list_.push_back(key, value);
            map_[key] = list_.tail;
        }
    }
    int get(int key){
        //Si el elemento está en la caché, se mueve a la cola
        if (map_.find(key) != map_.end()){
            //Mover el nodo que contiene el número a la cola
            list_.kick_back(map_[key]);
            return map_[key]->value;
        }
        //Si no, se devuelve -1
        else return -1;
    }
};

#endif //CACHE_H