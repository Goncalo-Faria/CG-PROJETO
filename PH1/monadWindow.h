#ifndef monadWindow_h
#define monadWindow_h

typedef struct monad *MonadWindow;

MonadWindow mkMonadWindow();

MonadWindow mkMonadWindow(MonadWindow mold);

void unmkMonadWindow(MonadWindow m);

void monadRotate(MonadWindow m, double angle, double vx, double vy, double vz);

void monadAgregate(MonadWindow a, MonadWindow b);

void monadTranslate(MonadWindow m, double x, double y, double z);

void monadScale(MonadWindow m, double vx, double vy, double vz );

void monadPoint(MonadWindow m, double x, double y, double z);

void monadTriangle(MonadWindow m, double angle, double difs);

void monadStacker(MonadWindow reference, int points, int stacks, double (*f)(double));

void monadHyperplane(MonadWindow reference, int divisions);

void monadCube(MonadWindow reference, int divisions);

void monadCircle(MonadWindow reference,int points);

void monadTrace(MonadWindow m, char* filename, char* figure);


#endif
