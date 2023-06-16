#include <iostream>
#include "eigen\Dense"
#include <vector>
#include <cmath>
#include <fstream>
#include <random>
#include <ctime>

using namespace std;

struct Point{
    float x;
    float y;
    float z;
    Point(float x=0, float y=0, float z=0){
        this->x = x;
        this->y = y;
        this->z = z;
    }
    friend ostream& operator<<(ostream& os, const Point& p){
        os << p.x << ',' << p.y << ',' << p.z;
        return os;
    }
};

void generatePoints(vector<Point> &points, int n=200){
    for (int i = -n/2; i < n/2; i++){
        Point p(i, i * sin(rand() % 360), i * sin(rand() % 360));
        points.push_back(p);
    }
}

Eigen::MatrixXf vectToMatrix(vector<Point> &points){
    Eigen::MatrixXf matrix(points.size(), 3);
    for (int i = 0; i < points.size(); i++){
        matrix(i, 0) = points[i].x;
        matrix(i, 1) = points[i].y;
        matrix(i, 2) = points[i].z;
    }
    return matrix;
}

void getMaxEigenvect(vector<Point> &points, Point &eigenvect){
    Eigen::MatrixXf matrix = vectToMatrix(points);
    //Normalizar los datos
    Eigen::VectorXf mean = matrix.colwise().mean();
    matrix.rowwise() -= mean.transpose();
    //Calcular la matriz de covarianza
    Eigen::MatrixXf cov = matrix.transpose() * matrix;
    Eigen::EigenSolver<Eigen::MatrixXf> es(cov);
    Eigen::MatrixXf eigenvects = es.eigenvectors().real();
    Eigen::VectorXf maxEigenvect = eigenvects.col(0);
    eigenvect.x = maxEigenvect(0);
    eigenvect.y = maxEigenvect(1);
    eigenvect.z = maxEigenvect(2);
}

void savePoints(vector<Point> &points, Point &eigenvect, string filename){
    ofstream file(filename);
    file << eigenvect << endl;
    for (auto point : points){
        file << point << endl;
    }
    file.close();
}

int main(){
    string filename = "points.csv";
    srand(time(NULL));
    vector<Point> points; generatePoints(points);
    Point eigenvect; getMaxEigenvect(points, eigenvect);
    savePoints(points, eigenvect, filename);
}