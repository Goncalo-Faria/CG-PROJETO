#ifndef mat_h_
#define mat_h_

typedef struct mat *Mat;

Mat identity();
Mat zeros();
void freeMat(Mat t);

Mat matRx(float angle);
Mat matRy(float angle);
Mat matRz(float angle);

Mat matmul(Mat a, Mat b);
Mat matmul(float a[4][4], Mat b);

void matAssign(Mat r, Mat value);
void matAssign(float r[4][4], Mat value);
void matAssign(float r[4][4], float value[4][4]);

float bernstein(int i, int n, float t);

Mat matRotate(float angle, float vx, float vy, float vz);
Mat matTranslate(float x, float y, float z);
Mat matScale(float xaxis, float yaxis, float zaxis);

float * vecmul( Mat mat, float* vec );
float * vecmul( float mat[4][4] ,float * vec, int n);

float* crossVecProd(float *a, float *b);

Mat upsidemat(float*deriv , float*norm);

#endif