#ifndef RTREE_H
#define RTREE_H

#include<string>
#include<vector>

using namespace std;

float min(float a, float b){
    if (a < b){
        return a;
    }
    return b;
}
float max(float a, float b){
    if (a > b){
        return a;
    }
    return b;
}

template<int ndim>
struct Point{
    vector<float>coords;
    Point(void){
        this->coords.reserve(ndim);
        for (int i = 0; i < ndim; i++){
            coords.push_back(0);
        }
    }
    Point(float coords[ndim]){
        this->coords.reserve(ndim);
        for (int i = 0; i < ndim; i++){
            this->coords.push_back(coords[i]);
        }
    }
    Point(vector<float> coords){
        this->coords.reserve(ndim);
        for (int i = 0; i < ndim; i++){
            this->coords.push_back(coords[i]);
        }
    }
    //El operador [] está sobrecargado en el objeto vector
    float& operator[](int index){ return coords[index]; }
    ~Point(void){ delete &coords; }
    //getCoords retorna el punto con las coordenadas maximas o minimas de ambos puntos
    Point<ndim> getCoords(Point<ndim> other, float function(float, float)){
        float tmp[ndim];
        for (int i = 0; i < ndim; i++){
            tmp[i] = function(this->coords[i], other[i]);
        }
        return Point<ndim>(tmp);
    }
};

//number name attack defense speed height hp weight
class Pokemon{
    int number;
    string name;
    float height, hp, weight;
public:
    Pokemon(int number, string name, float height, float hp, float weight) : number(number), name(name), height(height), hp(hp), weight(weight) {}
    float getHeight(){ return height; }
    float getHp(){ return hp; }
    float getWeight(){ return weight; }
};

template< typename T, int ndim >
class Record {
private:
    T object; //aqui el pokemon (aqui está su tamaño)
    Point<ndim> coords; //coordenada donde está el pokemon
public:
    Record(T object, Point<ndim> coords) : object(object), coords(coords) {}
    T getObject(){ return object; }
    Point<ndim> getTuple(){ return this->coords; }
};

template< typename T, int ndim >
class Node { //Region //Rectangle //MBR
private:
    vector< Record<T,ndim> > records;
    vector< Node<T,ndim>* > children;
    //bottomLeft tiene las coordenadas minimas de todos los records
    //upperRight tiene las coordenadas maximas de todos los records
    Point<ndim> bottomLeft;
    Point<ndim> upperRight; 
    int maxRecords;
    int countRecords;
    bool isLeaf;  

public:
    Node(int maxRecords) : maxRecords(maxRecords), countRecords(0), isLeaf(true) {}

    bool insert(Record<T,ndim>  record){
        if (isLeaf){
            if (countRecords == 0){
                records.push_back(record);
                countRecords++;
                bottomLeft = upperRight = record.getTuple();
                return true;
            }
            else if (countRecords < maxRecords){
                records.push_back(record);
                countRecords++;
                bottomLeft = bottomLeft.getCoords(record.getTuple(), min);
                upperRight = upperRight.getCoords(record.getTuple(), max);
                return true;
            }
            else{
                //split
                pair< Node<T,ndim>*, Node<T,ndim>*> nodes = split();
                //pair a vector
                vector<Node<T,ndim>*> nodesv= {nodes.first, nodes.second};
                insertToBest(record, nodesv);
            }
        }
        else{
            insertToBest(record, children);
        }
        return true;
    }

    //insertToBest inserta en el nodo que se expanda menos de la lista de nodos pasados por parametro
    //si hay empate, inserta en el que tenga menor area
    bool insertToBest(Record<T,ndim>  record, vector<Node<T,ndim>*> nodes){
        float minExpansion = nodes[0]->getExpansion(record);
        int index = 0;
        for (int i = 1; i < nodes.size(); i++){
            float expansion = nodes[i]->getExpansion(record);
            if (expansion < minExpansion){
                minExpansion = expansion;
                index = i;
            }
            else if (expansion == minExpansion){
                if (nodes[i]->getArea() < nodes[index]->getArea()){
                    index = i;
                }
            }
        }
        return nodes[index]->insert(record);
    }

    int getCountRecords(){
        return countRecords;
    }
    int getCountChildren(){
        return children.size();
    }
    //getExpansion retorna el area que se expandiria si se insertara el record
    float getExpansion(Record<T,ndim>  record){
        float expansion = 1;
        for (int i = 0; i < ndim; i++){
            float min = bottomLeft[i];
            float max = upperRight[i];
            if (record.getTuple()[i] < min){
                min = record.getTuple()[i];
            }
            else if (record.getTuple()[i] > max){
                max = record.getTuple()[i];
            }
            expansion *= max - min;
        }
        return expansion - getArea();
    }

    float getArea(){
        float area = 1;
        for (int i = 0; i < ndim; i++){
            area *= upperRight[i] - bottomLeft[i];
        }
        return area;
    }

    //insert utilizado principalmente por RTree
    bool insert(Node<T,ndim>* node1, Node<T,ndim>* node2= NULL){
        //Insertar los nodos en el nodo padre
        children.push_back(node1);
        if (node2 != NULL){
            children.push_back(node2);
        }
        return true;
    }

    pair< Node<T,ndim>*, Node<T,ndim>*> split(){
        pair< Node<T,ndim>*, Node<T,ndim>*> nodes;
        nodes.first = new Node<T,ndim>(maxRecords);
        nodes.second = new Node<T,ndim>(maxRecords);
        //Insertar a los más cercanos a los extremos de los nodos bottomLeft y upperRight
        //Si un nodo alcanza la mitad de los records, insertar el resto en el otro nodo
        for (auto record: records){
            if (nodes.first->countRecords < maxRecords/2){
                nodes.first->insert(record);
            }
            else if (nodes.second->countRecords < maxRecords/2){
                nodes.second->insert(record);
            }
            else{
                if (nodes.first->getExpansion(record) < nodes.second->getExpansion(record)){
                    nodes.first->insert(record);
                }
                else{
                    nodes.second->insert(record);
                }
            }
        }
        return nodes;
    }

};

template< typename T, int ndim >
class RTree {
private:
    Node<T,ndim> *root;
    int maxRecords;
public:
    RTree(int maxRecords) : root(new Node<T,ndim>(maxRecords)), maxRecords(maxRecords) {}
    bool insert( Record<T,ndim> record ){
        //Si el nodo se divide, se crea un nuevo nodo y se inserta en el root
        if (root->getCountRecords() == maxRecords){
            Node<T,ndim> *newRoot = new Node<T,ndim>(maxRecords);
            bool done= newRoot->insert(root);
            root = newRoot;
            return done;
        }
        return root->insert(record);
    }
    int getNumberOfNodes(){
        return root->getCountChildren();
    }
};

#endif