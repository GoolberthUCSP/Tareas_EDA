#include "binary_tree.h"
#include "avl_tree.h"
#include <random>
#include <ctime>
#include <set>

#define SIZE 100000000 //Tamaño del vector
#define N_POINTS 50000000 //Número de puntos
#define MAX_INT 2000000000 //Máximo entero
#define OUTPUT "points.bin" //Nombre del fichero

using namespace std;

int randrange(int min, int max){
    return rand()%(max-min+1)+min;
}

void genPoints(vector<int> &v, int n){
    //Genera n puntos random entre 0 y 2000M
    //Los puntos no se repetirán
    set<int> s;
    int x= 1, n_ranges= 10000;
    int min = 1, max= MAX_INT/n_ranges, step = MAX_INT/n_ranges;
    int step_n = n/n_ranges;
    while (s.size() < n){
        //Insertar en rangos para evitar colisiones
        if (s.size() >= step_n*x){
            min = max+1;
            max += step;
            x++;
        }
        s.insert(randrange(min, max));
    }
    int i = 0;
    for (auto it = s.begin(); it != s.end(); it++){
        v[i++] = *it;
    }
}

//Se guardarán los datos del vector en un fichero .bin
void savePoints(vector<int> &v){
    //Guardar 100M de enteros en un fichero .bin
    ofstream file;
    file.open(OUTPUT, ios::binary);
    if(!file.is_open()){
        cout << "Error al abrir el fichero" << endl;
    }
    for(int i=0; i<v.size(); i++){
        file.write((char*)&v[i], sizeof(int));
    }
    file.close();
}

int main(){
    srand(time(NULL));
    Node *avl_tree= NULL;
    //Vector de 100M de enteros
    vector<int> v;
    v.assign(SIZE, 0);
    genPoints(v, N_POINTS);
    for (int i = 0; i < N_POINTS; i++){
        avl_tree = insertNode(avl_tree, v[i]);
    }
    vector<int> keys = getTreeKeys(avl_tree);
    for (int i = 0; i < keys.size(); i++){
        v[i] = keys[i];
    }
    savePoints(v);
}