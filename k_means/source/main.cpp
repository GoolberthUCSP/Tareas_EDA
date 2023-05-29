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
        int pos = line.find(",");
        double x = stod(line.substr(0, pos));
        double y = stod(line.substr(pos + 1));
        points.push_back(point(x, y));
    }
    file.close();
}


int main(){
    vector<point> points;
    load("puntos.txt", points);
    cout << "size: " << points.size() << endl;
    k_means kmeans(points, 2);
    vector<point> solution= kmeans.get_solution();
    for (auto i: solution){
        cout << i << endl;
    }
}