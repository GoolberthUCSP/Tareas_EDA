#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

using namespace std;

omp_lock_t lock;

#define STEP_SZ 10
#define H_WINDOWS 800
#define W_WINDOWS 800
#define SPEED 1  // check https://docs.python.org/3/library/turtle.html#turtle.speed

FILE *out;

void print_python_head(){
    fprintf(out,"import turtle\n");
    fprintf(out,"turtle.penup()\n");
    //fprintf(out, "turtle.goto(-%d, %d)\n", H_WINDOWS/2 - STEP_SZ, W_WINDOWS/2 - STEP_SZ);
    fprintf(out,"turtle.pendown()\n");
    fprintf(out,"turtle.setup(%d, %d)\n", H_WINDOWS, W_WINDOWS);
    //fprintf(out, "turtle.speed(%d)\n", SPEED);
	fprintf(out, "turtle.tracer(0)\n");
}

void print_python_end(){
	fprintf(out, "turtle.update()\n"); 
    fprintf(out, "screen = turtle.Screen()\n"); // EXIT ON CLICK
    fprintf(out, "screen.exitonclick()\n"); // EXIT ON CLICK
}

void hilbert_curve(int n, int angle) {
    if( n==0 ) return;
    fprintf(out,"turtle.right(%d)\n", angle);
    
    hilbert_curve(n-1, -angle);
    
    fprintf(out,"turtle.forward(%d)\n", STEP_SZ);
    fprintf(out,"turtle.left(%d)\n", angle);
    
    hilbert_curve(n-1, angle);

    fprintf(out,"turtle.forward(%d)\n", STEP_SZ);
    
    hilbert_curve(n-1, angle);

    fprintf(out,"turtle.left(%d)\n", angle);
    fprintf(out,"turtle.forward(%d)\n", STEP_SZ);

    hilbert_curve(n-1, -angle);
    
    fprintf(out,"turtle.right(%d)\n", angle);
}

void hilbert_subprocess(int n, int angle, std::stringstream& buffer) {
    if (n == 0) return;
    buffer << "turtle.right(" << angle << ")\n";

    hilbert_subprocess(n - 1, -angle, buffer);

    buffer << "turtle.forward(" << STEP_SZ << ")\n";
    buffer << "turtle.left(" << angle << ")\n";

    hilbert_subprocess(n - 1, angle, buffer);

    buffer << "turtle.forward(" << STEP_SZ << ")\n";

    hilbert_subprocess(n - 1, angle, buffer);

    buffer << "turtle.left(" << angle << ")\n";
    buffer << "turtle.forward(" << STEP_SZ << ")\n";

    hilbert_subprocess(n - 1, -angle, buffer);

    buffer << "turtle.right(" << angle << ")\n";
}

std::string generate_hilbert_curve(int n) {
    std::stringstream buffer;

    #pragma omp parallel for
    for (int i = 0; i < 4; ++i) {
        std::stringstream local_buffer;
        hilbert_subprocess(n - 1, (i % 2 == 0) ? 90 : -90, local_buffer);

        #pragma omp critical
        buffer << local_buffer.rdbuf();
    }

    return buffer.str();
}

int main(int argc, char const *argv[]) {
    out = fopen("hilbert_turtle.py", "w+");
    print_python_head();

    int order = 5;
    int angle = 90;
    
    std::string buffer = generate_hilbert_curve(order);
    fprintf(out, "%s\n", buffer.c_str());

    print_python_end();
    fclose(out);
    return 0;
}
