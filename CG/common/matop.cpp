#include "matop.h"
#include <math.h>
#include <cstdlib>
#include <cstring>

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

float* crossVecProd(float *a, float *b) {

    float * res = (float*)malloc( sizeof(float)*3 );

    res[0] = a[1]*b[2] - a[2]*b[1];
    res[1] = a[2]*b[0] - a[0]*b[2];
    res[2] = a[0]*b[1] - a[1]*b[0];

    return res;
}

float ** upsidemat(float*deriv , float*norm){


    float* z = crossVecProd(deriv,norm);

    float* yn = crossVecProd(z,deriv);


    float lyn = sqrt(yn[0]*yn[0] + yn[1]*yn[1] + yn[2]*yn[2]);
    float lz = sqrt(z[0]*z[0]  + z[1]*z[1] + z[2]*z[2]);
    float lderiv = sqrt(deriv[0]*deriv[0] + deriv[1]*deriv[1] + deriv[2]*deriv[2]);

    for(int i=0; i< 3; i++){
        yn[i]= yn[i]/lyn;
        z[i]= z[i]/lz;
        deriv[i]= deriv[i]/lderiv;
    }

    norm[0]=yn[0];
    norm[1]=yn[1];
    norm[2]=yn[2];

    free(yn);

    float** mat = (float**)malloc(sizeof(float*)*4);

    mat[0] = (float*)malloc(sizeof(float)*4);

    mat[0][0] = deriv[0]; mat[0][1]=norm[0]; mat[0][2]=z[0]; mat[0][3]=0;

    mat[1] = (float*)malloc(sizeof(float)*4);

    mat[1][0] = deriv[1]; mat[1][1]=norm[1]; mat[1][2]=z[1]; mat[1][3]=0;

    mat[2] = (float*)malloc(sizeof(float)*4);

    mat[2][0] = deriv[2]; mat[2][1]=norm[2]; mat[2][2]=z[2]; mat[2][3]=0;

    mat[3] = (float*)malloc(sizeof(float)*4);

    mat[3][0] = 0; mat[3][1]=0; mat[3][2]=0; mat[3][3]=1;

    free(z);
    return mat;

}

float * vecmul( float** mat ,float * vec){

    float * r = (float*)malloc( sizeof(float)*3);

    for(int i=0; i< 3; i++){
        r[i] = 0;
        for(int j=0; j< 4; j++)
            r[i] += mat[i][j] * vec[j];
    }

    return r;
}

float * vecmul( float mat[4][4] ,float * vec, int n){

    float * r = (float*)malloc( sizeof(float)*n);

    for(int i=0; i< n; i++){
        r[i] = 0;
        for(int j=0; j< 4; j++)
            r[i] += mat[i][j] * vec[j];
    }

    return r;
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
    float l = sqrt(vx*vx + vy*vy + vz*vz);

    float rx = vx*angle/l;
    float ry = vy*angle/l;
    float rz = vz*angle/l;

    float** mrx = matRx(rx);
    float** mry = matRy(ry);
    float** mrz = matRz(rz);

    float** mr1 = matmul(mrx,mry);
    float** mr2 = matmul(mr1,mrz);
    freeMat(mr1);

    return mr2;
}

float ** matTranslate(float x, float y, float z){
    float** t = identity();

    t[0][3] = x;
    t[1][3] = y;
    t[2][3] = z;

    return t;
}

float** matScale(float xaxis, float yaxis, float zaxis){
    float** t = identity();

    t[0][0] = xaxis;
    t[1][1] = yaxis;
    t[2][2] = zaxis;

    return t;
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