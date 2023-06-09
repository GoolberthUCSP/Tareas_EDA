#include <iostream>
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
    fprintf(out, "turtle.goto(-%d, %d)\n", H_WINDOWS/2 - STEP_SZ, W_WINDOWS/2 - STEP_SZ);
    fprintf(out,"turtle.pendown()\n");
    fprintf(out,"turtle.setup(%d, %d)\n", H_WINDOWS, W_WINDOWS);
    fprintf(out, "turtle.speed(%d)\n", SPEED);
	fprintf(out, "turtle.tracer(0)\n");
}

void print_python_end(){
	fprintf(out, "turtle.update()\n"); 
    fprintf(out, "screen = turtle.Screen()\n"); // EXIT ON CLICK
    fprintf(out, "screen.exitonclick()\n"); // EXIT ON CLICK
}

// Función recursiva para generar la curva de Hilbert y escribir en el buffer
void hilbert_curve(int n, int angle, stringstream& buffer) {
    if (n == 0) return;
    buffer << "turtle.right(" << angle << ")\n";
    hilbert_curve(n - 1, -angle, buffer);
    buffer << "turtle.forward(" << STEP_SZ << ")\n";
    buffer << "turtle.left(" << angle << ")\n";
    hilbert_curve(n - 1, angle, buffer);
    buffer << "turtle.forward(" << STEP_SZ << ")\n";
    hilbert_curve(n - 1, angle, buffer);
    buffer << "turtle.left(" << angle << ")\n";
    buffer << "turtle.forward(" << STEP_SZ << ")\n";
    hilbert_curve(n - 1, -angle, buffer);
    buffer << "turtle.right(" << angle << ")\n";
}

// Función para generar la curva de Hilbert paralelizada
void hilbert_curve_parallel(int n, int angle) {
    stringstream buffer;
    stringstream buffers[4];
    int angles[4] = {-angle, angle, angle, -angle};
    buffers[0] << "turtle.right(" << angle << ")\n";
    buffers[1] << "turtle.forward(" << STEP_SZ << ")\n";
    buffers[1] << "turtle.left(" << angle << ")\n";
    buffers[2] << "turtle.forward(" << STEP_SZ << ")\n";
    buffers[3] << "turtle.left(" << angle << ")\n";
    buffers[3] << "turtle.forward(" << STEP_SZ << ")\n";
    //Paralelización del for
    #pragma omp parallel for 
    for (int i = 0; i < 4; ++i) {
        hilbert_curve(n - 1, angles[i], buffers[i]);
        #pragma omp critical
        buffer << buffers[i].str();
    }
    buffer << "turtle.right(" << angle << ")\n";
    // Unión de soluciones parciales
    fprintf(out, "%s\n", buffer.str().c_str());
}

int main(int argc, char const *argv[]) {
    out = fopen("hilbert_turtle.py", "w+");
    print_python_head();

    int order = 5;
    int angle = 90;

    hilbert_curve_parallel(order, angle);

    print_python_end();
    fclose(out);
    return 0;
}
