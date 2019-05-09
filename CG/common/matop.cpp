#include "matop.h"
#include <math.h>
#include <cstdlib>
#include <cstring>

typedef struct mat{
    float ** mat;
} *Mat;

Mat identity(){
    Mat m = (Mat)malloc(sizeof(struct mat));
    m->mat = (float**)malloc(sizeof(float*)*4);
    for(int i = 0; i< 4; i++) {
        m->mat[i] = (float*)malloc( sizeof(float) * 4 );
        for (int j = 0; j < 4; j++)
            m->mat[i][j] = (i == j);
    }

    return m;
}

Mat zeros(){
    Mat m = (Mat)malloc(sizeof(struct mat));
    m->mat = (float**)malloc(sizeof(float*)*4);
    for(int i = 0; i< 4; i++) {
        m->mat[i] = (float*)malloc( sizeof(float) * 4 );
        for (int j = 0; j < 4; j++)
            m->mat[i][j] = 0;
    }

    return m;
}

void freeMat(Mat t){

    for(int i = 0; i< 4; i++)
        free(t->mat[i]);

    free(t->mat);
    free(t);
}

Mat matRx(float angle){
    Mat m = identity();

    float rad = (M_PI/180.0)*angle;

    m->mat[1][1] = cos(rad);
    m->mat[1][2] = -sin(rad);
    m->mat[2][1] = sin(rad);
    m->mat[2][2] = cos(rad);

    return m;
}

Mat matRy(float angle){
    Mat t = identity();

    float rad = (M_PI/180.0)*angle;

    t->mat[0][0] = cos(rad);
    t->mat[0][2] = sin(rad);
    t->mat[2][0] = -sin(rad);
    t->mat[2][2] = cos(rad);

    return t;
}

Mat matRz(float angle){
    Mat t = identity();

    float rad = (M_PI/180.0)*angle;

    t->mat[0][0] = cos(rad);
    t->mat[0][1] = -sin(rad);
    t->mat[1][0] = sin(rad);
    t->mat[1][1] = cos(rad);

    return t;
}

Mat matmul(Mat a, Mat b){
    Mat result = zeros();

    for( int i = 0; i<4; i++ )
        for(int j = 0;j<4; j++)
            for(int k = 0; k<4; k++)
                result->mat[i][j] += a->mat[i][k] *  b->mat[k][j];

    return result;
}

float* crossVecProd(float *a, float *b) {

    float * res = (float*)malloc( sizeof(float)*3 );

    res[0] = a[1]*b[2] - a[2]*b[1];
    res[1] = a[2]*b[0] - a[0]*b[2];
    res[2] = a[0]*b[1] - a[1]*b[0];

    return res;
}

Mat upsidemat(float*deriv , float*norm){


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

    Mat m = (Mat)malloc(sizeof(struct mat));
    m->mat = mat;

    return m;
}

float * vecmul( Mat mat ,float * vec){

    float * r = (float*)malloc( sizeof(float)*3);

    for(int i=0; i< 3; i++){
        r[i] = 0;
        for(int j=0; j< 4; j++)
            r[i] += mat->mat[i][j] * vec[j];
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


Mat matmul(float a[4][4], Mat b){
    Mat result = zeros();

    for( int i = 0; i<4; i++ )
        for(int j = 0;j<4; j++)
            for(int k = 0; k<4; k++)
                result->mat[i][j] += a[i][k] *  b->mat[k][j];

    return result;
}


Mat matRotate(float angle, float vx, float vy, float vz){
    float l = sqrt(vx*vx + vy*vy + vz*vz);

    float rx = vx*angle/l;
    float ry = vy*angle/l;
    float rz = vz*angle/l;

    Mat mrx = matRx(rx);
    Mat mry = matRy(ry);
    Mat mrz = matRz(rz);

    Mat mr1 = matmul(mrx,mry);
    Mat mr2 = matmul(mr1,mrz);

    freeMat(mr1);

    freeMat(mrx);
    freeMat(mry);
    freeMat(mrz);

    return mr2;
}

Mat matTranslate(float x, float y, float z){
    Mat t = identity();

    t->mat[0][3] = x;
    t->mat[1][3] = y;
    t->mat[2][3] = z;

    return t;
}

Mat matScale(float xaxis, float yaxis, float zaxis){
    Mat t = identity();

    t->mat[0][0] = xaxis;
    t->mat[1][1] = yaxis;
    t->mat[2][2] = zaxis;

    return t;
}

void matAssign(Mat r, Mat value){

    for( int i = 0; i<4; i++ )
        for(int j = 0;j<4; j++)
            r->mat[i][j] = value->mat[i][j];
}

void matAssign(float r[4][4], Mat value){

    for( int i = 0; i<4; i++ )
        for(int j = 0;j<4; j++)
            r[i][j] = value->mat[i][j];
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