#include "matop.h"
#include <math.h>
#include <cstdlib>

float** identity(){
    float**t = (float**)malloc(sizeof(float*)*4);
    for(int i = 0; i< 4; i++) {
        t[i] = (float*)malloc( sizeof(float) * 4 );
        for (int j = 0; j < 4; j++)
            t[i][j] = (i == j);
    }

    return t;
}

float** zeros(){
    float**t = (float**)malloc(sizeof(float*)*4);
    for(int i = 0; i< 4; i++) {
        t[i] = (float*)malloc( sizeof(float) * 4 );
        for (int j = 0; j < 4; j++)
            t[i][j] = 0;
    }

    return t;
}

void freeMat(float** t){

    for(int i = 0; i< 4; i++)
        free(t[i]);

    free(t);
}

float** matRx(float angle){
	float**t = identity();

	float rad = (M_PI/180.0)*angle;

	t[1][1] = cos(rad);
	t[1][2] = -sin(rad);
	t[2][1] = sin(rad);
	t[2][2] = cos(rad);

	return t;
}

float** matRy(float angle){
    float**t = identity();

	float rad = (M_PI/180.0)*angle;

	t[0][0] = cos(rad);
	t[0][2] = sin(rad);
	t[2][0] = -sin(rad);
	t[2][2] = cos(rad);

	return t;
}

float** matRz(float angle){
    float**t = identity();

	float rad = (M_PI/180.0)*angle;

	t[0][0] = cos(rad);
	t[0][1] = -sin(rad);
	t[1][0] = sin(rad);
	t[1][1] = cos(rad);

	return t;
}

float** matmul(float** a, float** b){
    float ** result = zeros();

    for( int i = 0; i<4; i++ )
        for(int j = 0;j<4; j++)
            for(int k = 0; k<4; k++)
                result[i][j] += a[i][k] *  b[k][j];

    return result;
}

float** matmul(float a[4][4], float** b){
    float ** result = zeros();

    for( int i = 0; i<4; i++ )
        for(int j = 0;j<4; j++)
            for(int k = 0; k<4; k++)
                result[i][j] += a[i][k] *  b[k][j];

    return result;
}

float** matRotate(float angle, float vx, float vy, float vz){
    float rx = vx*angle;
    float ry = vy*angle;
    float rz = vz*angle;

    float** mrx = matRx(rx);
    float** mry = matRy(ry);
    float** mrz = matRz(rz);

    float** mr1 = matmul(mrx,mry);
    float** mr2 = matmul(mr1,mrz);
    freeMat(mr1);

    return mr2;
}

void matAssign(float**r, float**value){

    for( int i = 0; i<4; i++ )
        for(int j = 0;j<4; j++)
            r[i][j] = value[i][j];
}

void matAssign(float r[4][4], float**value){

    for( int i = 0; i<4; i++ )
        for(int j = 0;j<4; j++)
            r[i][j] = value[i][j];
}

void matAssign(float r[4][4], float value[4][4] ){

    for( int i = 0; i<4; i++ )
        for(int j = 0;j<4; j++)
            r[i][j] = value[i][j];
}

int factorial(int n){ return (n < 2) ? 1 : n*factorial(n-1); }

float bernstein(int i, int n, float t){
	float r = (float) factorial(n) / (float) (factorial(i) * factorial(n - i));
	r *= pow(t,i);
	r *= pow(1-t,n-i);
	return r;
}