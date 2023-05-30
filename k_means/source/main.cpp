#include<iostream>
#include<fstream>
#include<string>
#include<vector>
#include "k_means.h"

using namespace std;



void load(string filename, vector<point> &points){
    ifstream file(filename);
    string line;
    while(getline(file, line)){
        int pos = line.find(" ");
        double x = stod(line.substr(0, pos));
        double y = stod(line.substr(pos + 1));
        points.push_back(point(x, y));
    }
    file.close();
}

void save(vector<vector<point>> &clusters, string filename){
    ofstream file(filename);
    file << "x,y,cluster" << endl;
    for(int i = 0; i < clusters.size(); i++){
        for(auto &j : clusters[i]){
            file << j << "," << i << endl;
        }
    }
    file.close();
}

int main(){
    vector<point> points;
    
    string filenames[] = {"../data/puntos_2_bloques.txt", "../data/puntos_5_bloques.txt"};
    string out_filenames[] = {"../data/k2.csv", "../data/k5.csv"};
    int ks[] = {2, 5};
    //Creación de los clusters y guardado de los resultados
    for(int i = 0; i < 2; i++){
        load(filenames[i], points);
        k_means kmeans(points, ks[i]);
        k_means_parallel kmeans_parallel(points, ks[i]);
        vector<vector<point>> clusters = kmeans_parallel.get_clusters();
        save(clusters, out_filenames[i]);
        cout << "K=" << ks[i] << endl;
        cout << "Sequential time: " << kmeans.get_time() << endl;
        cout << "Parallel time: " << kmeans_parallel.get_time() << endl;
        points.clear();
    }
    
    /*
    int ks[] = {2, 5, 10, 20, 50, 100};
    for(int i = 0; i < 6; i++){
        string filename = "../data/puntos_" +  to_string(ks[i]) + "_bloques.txt";
        load(filename, points);
        k_means kmeans(points, ks[i]);
        k_means_parallel kmeans_parallel(points, ks[i]);
        cout << "K=" << ks[i] << endl;
        cout << kmeans.get_time() << endl;
        cout << kmeans_parallel.get_time() << endl;
        points.clear();
    }
    */
}