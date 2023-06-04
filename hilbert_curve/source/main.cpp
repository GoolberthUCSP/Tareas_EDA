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
    fprintf(out, "turtle.goto(-%d, %d)\n", H_WINDOWS/2 - STEP_SZ, W_WINDOWS/2 - STEP_SZ);
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

void hilbert_subprocess(int n, int angle, stringstream &buffer) {
    if( n==0 ) return;
    buffer << "turtle.right(" << angle << ")\n";

    hilbert_subprocess(n-1, -angle, buffer);
    
    buffer << "turtle.forward(" << STEP_SZ << ")\n";
    buffer << "turtle.left(" << angle << ")\n";
    
    hilbert_subprocess(n-1, angle, buffer);

    buffer << "turtle.forward(" << STEP_SZ << ")\n";
    
    hilbert_subprocess(n-1, angle, buffer);

    buffer << "turtle.left(" << angle << ")\n";
    buffer << "turtle.forward(" << STEP_SZ << ")\n";

    hilbert_subprocess(n-1, -angle, buffer);
    
    buffer << "turtle.right(" << angle << ")\n";
}

void hilbert_curve_parallel(int n, int angle){
    // Hacer hilbert_curve() en 4 hilos
    // Cada hilo debe hacer hilbert_curve() con un ángulo diferente
    // El resultado de cada hilo debe ser cargado a un buffer diferente
    // Al final, los 4 buffers deben ser concatenados en un solo buffer
    // El buffer resultante debe ser guardado en out
    stringstream ss[4];
    int angles[4] = {angle, -angle, -angle, angle};
    #pragma omp parallel for
    for(int i=0; i<4; i++){
        hilbert_subprocess(n-1, angles[i], ss[i]);
    }

    // Concatenar buffers
    stringstream buffer;
    for(int i=0; i<4; i++){
        buffer << ss[i].str();
    }
    // Guardar buffer en out
    fprintf(out, "%s", buffer.str().c_str());
    // Limpiar buffers
    for(int i=0; i<4; i++){
        ss[i].str("");
    }
    buffer.str("");
}

int main(int argc, char const *argv[]) {
    out = fopen("hilbert_turtle.py", "w+");
    print_python_head();

    int order = 4;
    int angle = 90;
    
    hilbert_curve_parallel(order, angle);

    print_python_end();
    fclose(out);
    return 0;
}
