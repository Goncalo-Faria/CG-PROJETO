#ifndef monadWindow_h
#define monadWindow_h

typedef struct monad *MonadWindow;

MonadWindow mkMonadWindow();

MonadWindow mkMonadWindow(MonadWindow mold);

void unmkMonadWindow(MonadWindow m);

void monadRotate(MonadWindow m, float angle, float vx, float vy, float vz);

void monadAgregate(MonadWindow a, MonadWindow b);

void monadTranslate(MonadWindow m, float x, float y, float z);

void monadScale(MonadWindow m, float vx, float vy, float vz );

void monadPoint(MonadWindow m, float x, float y, float z);

void monadTriangle(MonadWindow m, float angle);

void stacker(MonadWindow reference,int points, int stacks, float h, float (*f)(float,float) );

void plataform(MonadWindow reference, int points,float h, float bottomradius, float topradius);

void print_trace(MonadWindow m, char* filename, char* figure);

#endif
