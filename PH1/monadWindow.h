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

void monadTriangle(MonadWindow m, float angle, float difs);

void monadStacker(MonadWindow reference, int points, int stacks, float (*f)(float));

void monadHyperplane(MonadWindow reference, int divisions);

void monadCube(MonadWindow reference, int divisions);

void monadCircle(MonadWindow reference,int points);

void monadTrace(MonadWindow m, char* filename, char* figure);

#endif
