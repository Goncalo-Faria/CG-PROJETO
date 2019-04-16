#ifndef mat_h_
#define mat_h_

float** identity();
float** zeros();
void freeMat(float** t);

float** matRx(float angle);
float** matRy(float angle);
float** matRz(float angle);

float** matmul(float** a, float** b);
float** matmul(float a[4][4], float** b);

void matAssign(float**r, float**value);
void matAssign(float r[4][4], float**value);
void matAssign(float r[4][4], float value[4][4]);

float bernstein(int i, int n, float t);

float** matRotate(float angle, float vx, float vy, float vz);
float ** matTranslate(float x, float y, float z);
float** matScale(float xaxis, float yaxis, float zaxis);

float * vecmul(float**mat, float* vec);
float * vecmul( float mat[4][4] ,float * vec, int n);

float* crossVecProd(float *a, float *b);

float ** upsidemat(float*deriv , float*norm);

#endif