#include "avltree.h"
#include "binary_tree.h"
#include <random>
#include <queue>
#include <ctime>
#include <set>

#define SIZE 100000000 //Tamaño del vector
#define N_POINTS 50000000 //Número de puntos
#define MAX_INT 2000000000 //Máximo entero a generar

using namespace std;

int randrange(int min, int max){
    return rand()%(max-min+1)+min;
}

void genPoints(int *arr, int n= N_POINTS){
    //Genera n puntos random entre 1 y 2000M
    //Los puntos no se repetirán
    bin_tree tree;
    set<int> s;
    int x= 1, n_ranges= 10000;
    int min = 1, max= MAX_INT/n_ranges, step = max;
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
        arr[i++] = *it;
    }
}

void avlToBintree(Node *root, bin_tree &tree) {
  if (root == NULL) return;
  queue<Node *> q;
  q.push(root);
  while (!q.empty()) {
    Node *node = q.front();
    q.pop();
    
    //Insertar en el arbol binario
    tree.insert(node->key);

    if (node->left != NULL)
      q.push(node->left);
    if (node->right != NULL)
      q.push(node->right);
  }
}

int main(){
    srand(time(NULL));

    Node *avltree= NULL;
    bin_tree bintree;

    //Vector de 50M de enteros
    int *arr = new int[N_POINTS];
    genPoints(arr);
    for (int i = 0; i < N_POINTS; i++){
        avltree = insertNode(avltree, arr[i]);
    }
    //Se insertan los valores del arbol avl en el arbol binario
    avlToBintree(avltree, bintree);
    
    delete[] arr;
}