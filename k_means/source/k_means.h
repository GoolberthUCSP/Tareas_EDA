#include<thread>
#include<vector>
#include<cmath>
#include<random>
#include<time.h>
#define N_TH std::thread::hardware_concurrency()
#define N_ITER 10000

using namespace std;



struct point{
    point(double x= 0, double y= 0){
        this->x = x;
        this->y = y;
    }
    double distance(point &other){
        return sqrt(pow(x - other.x, 2) + pow(y - other.y, 2));
    }
    point operator+(point &other){
        return point(x + other.x, y + other.y);
    }
    point operator/(double n){
        return point(x / n, y / n);
    }
    bool operator!=(point &other){
        return x != other.x || y != other.y;
    }
    friend ostream& operator<<(ostream& os, const point& p){
        os << '(' << p.x << ';' << p.y << ')';
        return os;
    }
    double x, y;
};

class parent{
public:
    parent(vector<point> &points, int k){
        srand(time(NULL));
        this->points = points;
        this->k = k;
        this->clusters.resize(k);
        this->centroids.resize(k);
        this->create_centroids();
    }
    void create_centroids(){
        for(int i = 0; i < k; i++){
            int index = rand() % points.size();
            centroids[i] = points[index];
        }
    }
    void assign_to_clusters(int start, int end){
        for(int i = start; i < end; i++){
            int min_index = 0;
            double min_distance = points[i].distance(centroids[0]);
            for(int j = 1; j < k; j++){
                double distance = points[i].distance(centroids[j]);
                if(distance < min_distance){
                    min_distance = distance;
                    min_index = j;
                }
            }
            clusters[min_index].push_back(points[i]);
        }
    }
    void calc_centroid(int index){
        point sum;
        for(int i = 0; i < clusters[index].size(); i++){
            sum = sum + clusters[index][i];
        }
        centroids[index] = sum / clusters[index].size();
    }
    vector<point> &get_solution(){
        return centroids;
    }
protected:
    vector<point> points;
    vector<vector<point>> clusters;
    vector<point> centroids;
    int k;
};

class k_means: public parent{
public:
    k_means(vector<point> &points, int k): parent(points, k){
        this->solve();
    }
    void solve(int iterations = N_ITER){
        for(int it = 0; it < iterations; it++){
            clusters.clear();
            clusters.resize(k);
            //Guardar centroides anteriores
            vector<point> old_centroids = centroids;
            //Asignar puntos a clusters
            assign_to_clusters(0, points.size());
            //Calcular centroides
            for(int i = 0; i < k; i++){
                calc_centroid(i);
            }
            //Verificar si los centroides cambiaron
            bool changed = false;
            for(int i = 0; i < k; i++){
                if(centroids[i] != old_centroids[i]){
                    changed = true;
                    break;
                }
            }
            if(!changed){
                break;
            }
        }
    }
};

class k_means_parallel: public parent{
public:
    k_means_parallel(vector<point> &points, int k) : parent(points, k){
        this->solve();
    }
    void solve(int iterations = N_ITER){
        vector<thread> threads;
        for(int it = 0; it < iterations; it++){
            clusters.clear();
            clusters.resize(k);
            //Guardar centroides anteriores
            vector<point> old_centroids = centroids;
            //Asignar puntos a clusters
            for(int i = 0; i < N_TH; i++){
                threads.push_back(thread(&k_means_parallel::assign_to_clusters, this, i * points.size() / N_TH, (i + 1) * points.size() / N_TH));
            }
            for(int i = 0; i < N_TH; i++){
                threads[i].join();
            }
            //Liberar threads
            threads.clear();
            //Calcular centroides
            for(int i = 0; i < k; i++){
                threads.push_back(thread(&k_means_parallel::calc_centroid, this, i));
            }
            for(int i = 0; i < k; i++){
                threads[i].join();
            }
            threads.clear();
            //Verificar si los centroides cambiaron
            bool changed = false;
            for(int i = 0; i < k; i++){
                if(centroids[i] != old_centroids[i]){
                    changed = true;
                    break;
                }
            }
            if(!changed){
                break;
            }
        }
    }
};