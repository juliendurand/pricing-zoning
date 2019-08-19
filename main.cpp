#include <algorithm>
#include <functional>
#include <iostream>
#include <iterator>
#include <cmath>
#include <random>
#include <stdexcept>
#include <unistd.h>
#include <vector>


struct Point{
    int32_t idx;
    int32_t x;
    int32_t y;
    float w;
    float pred;
    float target;

    Point(): idx(0), x(0), y(0), w(1), pred(0), target(0) {}

    bool operator < (const Point& p) const
    {
        return (x < p.x);
    }

    bool operator < (int x0) const
    {
        return (x < x0);
    }

    float dist(const Point& p){
        int a = x - p.x;
        int b = y - p.y;
        return std::sqrt(a * a + b * b);
    }
};

struct Result{
    int32_t idx;
    float distance;

    bool operator < (const Result& r) const
    {
        return (distance < r.distance);
    }
};


double averageUniformK(const std::vector<Point>& points,
                int k, int current_k){
    double prediction = 0;
    double target = 0;
    double w = 0;
    for(int i = 0; i < current_k; i++){
        const Point& p = points[i];
        prediction += p.pred;
        target += p.target;
        w += p.w;

        if(w >= k){
            //std::cout << target << " " << prediction << " " << target / (prediction + 0.00001) << std::endl;
            return target / (prediction + 0.00001);
        }
    }
    throw std::invalid_argument("averageK : Not enougth weight");
}


double averageGaussianK(const std::vector<Point>& points,
                int k, int current_k){
    double prediction = 0;
    double target = 0;
    double w = 0;
    for(int i = 0; i < current_k; i++){
        const Point& p = points[i];
        prediction += p.pred;
        target += p.target;
        w += p.w;

        if(w >= k){
            //std::cout << target << " " << prediction << " " << target / (prediction + 0.00001) << std::endl;
            return target / (prediction + 0.00001);
        }
    }
    throw std::invalid_argument("averageK : Not enougth weight");
}


int main(int argc, char** argv){
    // First create an instance of an engine and distribution.
    std::random_device rnd_device;
    std::mt19937 mersenne_engine(rnd_device());
    std::uniform_int_distribution<int> distribution(1, 100000);

    int size = 1000000;

    std::vector<Point> points(size);
    for(int i = 0; i < size; i++){
        points[i].idx = i;
        points[i].x = distribution(mersenne_engine);
        points[i].y = distribution(mersenne_engine);
    }
    std::vector<Point> points_idx = points;
    std::sort(points_idx.begin(), points_idx.end());

    std::vector<Result> selection(size);
    std::vector<Point> results(size);

    std::cout << "start" << std::endl;

    double rr = 0;
    int r = 10;
    int k = 10000;
    double w = 0;
    for(int i = 100000; i < 110000; i++){
        //std::cout << "point " << i << " " << current_k << std::endl;
        Point pivot = points[i];
        int current_k = 0;
        w = -1;
        while(w < k || w > 1.2 * k){
            //std::cout << "current_k " << current_k << std::endl;
            if(w == 0){
                //std::cout << r << " " << current_k << std::endl;
                r *= 10;
            }
            else if(w > 0){
                r *= std::sqrt(1.1 * float(k) / w);
                //std::cout << "change r " << r << " " << current_k << std::endl;
            }
            auto start = std::lower_bound(points_idx.begin(), points_idx.end(), pivot.x - r);
            auto end = std::lower_bound(points_idx.begin(), points_idx.end(), pivot.x + r);
            current_k = 0;
            w = 0;
            for(auto j = start; j < end; j++){
                if(j->y > pivot.y - r && j->y < pivot.y + r){
                    float distance = j->dist(pivot);
                    if(distance < r){
                        w += j->w;
                        selection[current_k++] = Result{j->idx, distance};
                    }
                }
            }
        }
        std::sort(selection.begin(), selection.begin() + current_k);
        for(int j = 0; j < current_k; j++){
            results[j] = points[selection[j].idx];
        }

        rr += averageK(results, 100, current_k);
        rr += averageK(results, 200, current_k);
        rr += averageK(results, 500, current_k);
        rr += averageK(results, 1000, current_k);
        rr += averageK(results, 2000, current_k);
        rr += averageK(results, 5000, current_k);
        rr += averageK(results, 10000, current_k);
        //std::cout << i << " " << r << " " << current_k << std::endl;
    }
    /*for(int i = 0; i < 10; i ++){
        Point p = points[selection[i].idx];
        std::cout << "Point[" << p.idx << "](" << p.x << ", " << p.y << ")" << std::endl;
        std::cout << selection[i].idx << " " << selection[i].distance << " " << p.x << " " << p.y << std::endl;
    }*/
    std::cout << r << " " << rr << " " << w << std::endl;
    std::cout << "end" << std::endl;
    return 0;
}
